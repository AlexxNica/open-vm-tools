/*********************************************************
 * Copyright (C) 1998 VMware, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation version 2.1 and no later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the Lesser GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.
 *
 *********************************************************/

/*
 * fileIOPosix.c --
 *
 *      Implementation of the file library host specific functions for linux.
 */


#if !defined(VMX86_TOOLS) && !defined(__APPLE__)
#define FILEIO_SUPPORT_ODIRECT
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#if defined __ANDROID__
#include <asm/unistd.h> // for __NR_SYSCALL_BASE
#endif
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#if !defined(N_PLAT_NLM) && defined(linux)
/*
 * These headers are needed to get __USE_LARGEFILE, __USE_LARGEFILE64,
 * and SYS__llseek.
 */
#   include <features.h>
#   include <linux/unistd.h>
#   include <syscall.h>
#endif
#include <sys/stat.h>
#include "su.h"

#if defined(__APPLE__)
#include "sysSocket.h" // Don't move this: it fixes a system header.
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/kauth.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <CoreFoundation/CoreFoundation.h>
#include <dlfcn.h>
#include <sys/xattr.h>
#else
#if defined(__FreeBSD__)
#include <sys/param.h>
#include <sys/mount.h>
#else
#if !defined(N_PLAT_NLM)
#include <sys/statfs.h>
#if !defined(sun)
#include <mntent.h>
#include <dlfcn.h>
#endif
#endif
#endif
#endif

/* Check for non-matching prototypes */
#include "vmware.h"
#include "str.h"
#include "err.h"
#include "posix.h"
#include "file.h"
#include "fileIO.h"
#include "fileInt.h"
#include "config.h"
#include "util.h"
#include "iovector.h"
#include "hostType.h"

#include "unicodeOperations.h"
#include "memaligned.h"

#if defined(__linux__)
#include "hostinfo.h"
#endif

#if defined(__APPLE__)
#include <sys/sysctl.h>
#endif

/*
 * fallocate() is only supported since the glibc-2.8 and
 * linux kernel-2.6.23. Presently the glibc in our toolchain is 2.3.
 */
#if defined(__linux__)
   #if !defined(SYS_fallocate)
      #if defined(__i386__)
         #define SYS_fallocate 324
      #elif __x86_64__
         #define SYS_fallocate 285
      #elif __arm__
         #define SYS_fallocate (__NR_SYSCALL_BASE+352) // newer glibc value
      #endif
   #endif
   #if !defined(FALLOC_FL_KEEP_SIZE)
      #define FALLOC_FL_KEEP_SIZE 1
   #endif
#endif

static const unsigned int FileIO_SeekOrigins[] = {
   SEEK_SET,
   SEEK_CUR,
   SEEK_END,
};

static const int FileIO_OpenActions[] = {
   0,
   O_TRUNC,
   O_CREAT,
   O_CREAT | O_EXCL,
   O_CREAT | O_TRUNC,
};

/*
 * Options for FileCoalescing performance optimization
 */
typedef struct FilePosixOptions {
   Bool initialized;
   Bool aligned;
   Bool enabled;
   int countThreshold;
   int sizeThreshold;
   int aioNumThreads;
} FilePosixOptions;


static FilePosixOptions filePosixOptions;


/*
 *-----------------------------------------------------------------------------
 *
 * FileIOErrno2Result --
 *
 *      Convert a POSIX errno to a FileIOResult code.
 *
 * Results:
 *      The FileIOResult corresponding to the errno, FILEIO_ERROR by default.
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

static FileIOResult
FileIOErrno2Result(int error)  // IN: errno to convert
{
   switch (error) {
   case EEXIST:
      return FILEIO_OPEN_ERROR_EXIST;
   case ENOENT:
      return FILEIO_FILE_NOT_FOUND;
   case EACCES:
      return FILEIO_NO_PERMISSION;
   case ENAMETOOLONG:
      return FILEIO_FILE_NAME_TOO_LONG;
   case ENOSPC:
      return FILEIO_WRITE_ERROR_NOSPC;
   case EFBIG:
      return FILEIO_WRITE_ERROR_FBIG;
#if defined(VMX86_SERVER)
   case EBUSY:
      return FILEIO_LOCK_FAILED;
#endif
#if defined(EDQUOT)
   case EDQUOT:
      return FILEIO_WRITE_ERROR_DQUOT;
#endif
   default:
      Log("%s: Unexpected errno=%d, %s\n", __FUNCTION__,
          error, Err_Errno2String(error));
      return FILEIO_ERROR;
   }
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_OptionalSafeInitialize --
 *
 *      Initialize global state. If this module is called from a
 *      thread other than the VMX or VCPU threads, like an aioGeneric worker
 *      thread, then we cannot do things like call config. Do that sort
 *      of initialization here, which is called from a safe thread.
 *
 *      This routine is OPTIONAL if you do not call this module from a
 *      worker thread. The same initialization can be done lazily when
 *      a read/write routine is called.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

INLINE void
FileIO_OptionalSafeInitialize(void)
{
   if (!filePosixOptions.initialized) {
      filePosixOptions.enabled =
                            Config_GetBool(TRUE, "filePosix.coalesce.enable");

      /*
       * Aligned malloc starts failing to allocate memory during heavy I/O on
       * Linux. We're not sure why -- maybe we are running out of mmaps?
       * Turn it off by default for now.
       */

      filePosixOptions.aligned =
                           Config_GetBool(FALSE, "filePosix.coalesce.aligned");

      filePosixOptions.countThreshold =
                           Config_GetLong(5, "filePosix.coalesce.count");

      filePosixOptions.sizeThreshold =
                           Config_GetLong(16*1024, "filePosix.coalesce.size");

      filePosixOptions.aioNumThreads =
                           Config_GetLong(0, "aiomgr.numThreads");

      filePosixOptions.initialized = TRUE;
   }
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_Invalidate --
 *
 *      Initialize a FileIODescriptor with an invalid value
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

void
FileIO_Invalidate(FileIODescriptor *fd)  // OUT:
{
   ASSERT(fd);

   (memset)(fd, 0, sizeof *fd);
   fd->posix = -1;
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_IsValid --
 *
 *      Check whether a FileIODescriptor is valid.
 *
 * Results:
 *      True if valid.
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

Bool
FileIO_IsValid(const FileIODescriptor *fd)  // IN:
{
   ASSERT(fd);

   return fd->posix != -1;
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_CreateFDPosix --
 *
 *      This function is for specific needs: for example, when you need
 *      to create a FileIODescriptor from an already open fd. Use only
 *      FileIO_* library functions on the FileIODescriptor from that point on.
 *
 *      Because FileIODescriptor struct is different on two platforms,
 *      this function is the only one in the file library that's
 *      platform-specific.
 *
 * Results:
 *      FileIODescriptor
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

FileIODescriptor
FileIO_CreateFDPosix(int posix,  // IN: UNIX file descriptor
                     int flags)  // IN: UNIX access flags
{
   FileIODescriptor fd;

   FileIO_Invalidate(&fd);

   if (flags & O_RDWR) {
      fd.flags |= (FILEIO_OPEN_ACCESS_READ | FILEIO_OPEN_ACCESS_WRITE);
   } else if (flags & O_WRONLY) {
      fd.flags |= FILEIO_OPEN_ACCESS_WRITE;
   } else if (flags & O_RDONLY) {
      fd.flags |= FILEIO_OPEN_ACCESS_READ;
   }

#if defined(O_SYNC) // Not available in FreeBSD tools build
   if (flags & O_SYNC) {
      fd.flags |= FILEIO_OPEN_SYNC;
   }
#endif

   fd.posix = posix;

   return fd;
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_GetVolumeSectorSize --
 *
 *      Get sector size of underlying volume.
 *
 * Results:
 *      Always 512, there does not seem to be a way to query sectorSize
 *      from filename.  But O_DIRECT boundary alignment constraint is
 *      always 512, so use that.
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

Bool
FileIO_GetVolumeSectorSize(ConstUnicode pathName,  // IN:
                           uint32 *sectorSize)     // OUT:
{
   ASSERT(sectorSize);

   *sectorSize = 512;

   return TRUE;
}


#if defined(__APPLE__)
/*
 *----------------------------------------------------------------------
 *
 * ProxySendResults --
 *
 *      Send the results of a open from the proxy.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
ProxySendResults(int sock_fd,     // IN:
                 int send_fd,     // IN:
                 int send_errno)  // IN:
{
   struct iovec iov;
   struct msghdr msg;
   char cmsgBuf[CMSG_SPACE(sizeof send_fd)];

   iov.iov_base = &send_errno;
   iov.iov_len = sizeof send_errno;

   if (send_fd == -1) {
      msg.msg_control = NULL;
      msg.msg_controllen = 0;
   } else {
      struct cmsghdr *cmsg;

      msg.msg_control = cmsgBuf;
      msg.msg_controllen = sizeof cmsgBuf;

      cmsg = CMSG_FIRSTHDR(&msg);

      cmsg->cmsg_level = SOL_SOCKET;
      cmsg->cmsg_len = CMSG_LEN(sizeof send_fd);
      cmsg->cmsg_type = SCM_RIGHTS;

      (*(int *) CMSG_DATA(cmsg)) = send_fd;

      msg.msg_controllen = cmsg->cmsg_len;
   }

   msg.msg_name = NULL;
   msg.msg_namelen = 0;
   msg.msg_iov = &iov;
   msg.msg_iovlen = 1;
   msg.msg_flags = 0;

   sendmsg(sock_fd, &msg, 0);
}


/*
 *----------------------------------------------------------------------
 *
 * ProxyReceiveResults --
 *
 *      Receive the results of an open from the proxy.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

static void
ProxyReceiveResults(int sock_fd,      // IN:
                    int *recv_fd,     // OUT:
                    int *recv_errno)  // OUT:
{
   int err;
   struct iovec iov;
   struct msghdr msg;
   struct cmsghdr *cmsg;
   uint8_t cmsgBuf[CMSG_SPACE(sizeof(int))];

   iov.iov_base = recv_errno;
   iov.iov_len = sizeof *recv_errno;

   msg.msg_control = cmsgBuf;
   msg.msg_controllen = sizeof cmsgBuf;
   msg.msg_name = NULL;
   msg.msg_namelen = 0;
   msg.msg_iov = &iov;
   msg.msg_iovlen = 1;

   err = recvmsg(sock_fd, &msg, 0);

   if (err <= 0) {
      *recv_fd = -1;
      *recv_errno = (err == 0) ? EIO : errno;

      return;
   }

   if (msg.msg_controllen == 0) {
      *recv_fd = -1;
   } else {
      cmsg = CMSG_FIRSTHDR(&msg); 

      if ((cmsg->cmsg_level == SOL_SOCKET) &&
          (cmsg->cmsg_type == SCM_RIGHTS)) {
         *recv_fd = *((int *) CMSG_DATA(cmsg));
      } else {
         *recv_fd = -1;
         *recv_errno = EIO;
      }
   }
}


/*
 *----------------------------------------------------------------------
 *
 * ProxyOpen --
 *
 *      Open a file via a proxy.
 *
 * Results:
 *      -1 on error
 *      >= 0 on success
 *
 * Side effects:
 *      errno is set on error
 *
 *----------------------------------------------------------------------
 */

static int
ProxyOpen(ConstUnicode pathName,  // IN:
          int flags,              // IN:
          int mode)               // IN:
{
   int err;
   pid_t pid;
   int fds[2];
   int proxyFD;

   int saveErrno = 0;

   if (pathName == NULL) {
      errno = EFAULT;
      return -1;
   }

   err = socketpair(AF_UNIX, SOCK_DGRAM, 0, fds);
   if (err == -1) {
      errno = ENOMEM; // Out of resources...
      return err;
   }

   pid = fork();
   if (pid == -1) {
      proxyFD = -1;
      saveErrno = ENOMEM; // Out of resources...
      goto bail;
   }

   if (pid == 0) { /* child:  use fd[0] */
      proxyFD = Posix_Open(pathName, flags, mode);

      ProxySendResults(fds[0], proxyFD, errno);

      _exit(0);
   } else {        /* parent: use fd[1] */
      ProxyReceiveResults(fds[1], &proxyFD, &saveErrno);

      waitpid(pid, &err, 0);
   }

bail:

   close(fds[0]);
   close(fds[1]);

   errno = saveErrno;

   return proxyFD;
}


/*
 *----------------------------------------------------------------------
 *
 * ProxyUse --
 *
 *      Determine is the open proxy is to be used.
 *
 * Results:
 *	0	Success, useProxy is set
 *	> 0	Failure (errno value); useProxy is undefined
 *
 * Side effects:
 *	None
 *
 *----------------------------------------------------------------------
 */

static int
ProxyUse(ConstUnicode pathName,  // IN:
         Bool *useProxy)         // IN:
{
   Unicode path;
   UnicodeIndex index;
   struct statfs sfbuf;
   struct stat statbuf;

   if (pathName == NULL) {
      errno = EFAULT;
      return -1;
   }

   if ((Posix_Lstat(pathName, &statbuf) == 0) &&
       S_ISLNK(statbuf.st_mode)) {
      *useProxy = TRUE;

      return 0;
   }

   /*
    * Construct the path to the directory that contains the filePath.
    */

   index = Unicode_FindLast(pathName, "/");

   if (index == UNICODE_INDEX_NOT_FOUND) {
      path = Unicode_Duplicate(".");
   } else {
      Unicode temp;

      temp = Unicode_Substr(pathName, 0, index + 1);
      path = Unicode_Append(temp, ".");
      Unicode_Free(temp);
   }

   /*
    * Attempt to obtain information about the testPath (directory
    * containing filePath).
    */

   if (Posix_Statfs(path, &sfbuf) == 0) {
      /*
       * The testPath exists; determine proxy usage explicitely.
       */

      *useProxy = strcmp(sfbuf.f_fstypename, "nfs") == 0 ?  TRUE : FALSE;
   } else {
      /*
       * A statfs error of some sort; Err on the side of caution.
       */

      *useProxy = TRUE;
   }

   Unicode_Free(path);

   return 0;
}


/*
 *----------------------------------------------------------------------
 *
 * PosixFileOpener --
 *
 *      Open a file. Use a proxy when creating a file or on NFS.
 *
 *      Why a proxy? The MacOS X 10.4.* NFS client interacts with our
 *      use of settid() and doesn't send the proper credentials on opens.
 *      This leads to files being written without error but containing no
 *      data. The proxy avoids all of this unhappiness.
 *
 * Results:
 *      -1 on error
 *      >= 0 on success
 *
 * Side effects:
 *      errno is set
 *
 *----------------------------------------------------------------------
 */

int
PosixFileOpener(ConstUnicode pathName,  // IN:
                int flags,              // IN:
                mode_t mode)            // IN:
{
   Bool useProxy;

   if ((flags & O_ACCMODE) || (flags & O_CREAT)) {
      int err;

      /*
       * Open for write and/or O_CREAT. Determine proxy usage.
       */ 
   
      err = ProxyUse(pathName, &useProxy);
      if (err != 0) {
         errno = err;

         return -1;
      }
   } else {
      /*
       * No write access, no need for a proxy.
       */

      useProxy = FALSE;
   }

   return useProxy ? ProxyOpen(pathName, flags, mode) :
                     Posix_Open(pathName, flags, mode);
}
#endif


/*
 *----------------------------------------------------------------------
 *
 * FileIO_Create --
 *
 *      Open/create a file; specify creation mode
 *
 * Results:
 *      FILEIO_SUCCESS on success: 'file' is set
 *      FILEIO_OPEN_ERROR_EXIST if the file already exists
 *      FILEIO_FILE_NOT_FOUND if the file is not present
 *      FILEIO_ERROR for other errors
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

FileIOResult
FileIO_Create(FileIODescriptor *file,   // OUT:
              ConstUnicode pathName,    // IN:
              int access,               // IN:
              FileIOOpenAction action,  // IN:
              int mode)                 // IN: mode_t for creation
{
   uid_t uid = -1;
   int fd = -1;
   int flags = 0;
   int error;
   FileIOResult ret;

   ASSERT(file);

   if (pathName == NULL) {
      errno = EFAULT;

      return FILEIO_ERROR;
   }

   ASSERT(!FileIO_IsValid(file));
   ASSERT(file->lockToken == NULL);
   ASSERT_ON_COMPILE(FILEIO_ERROR_LAST < 16); /* See comment in fileIO.h */

#if defined(__APPLE__)
   if (access & FILEIO_OPEN_EXCLUSIVE_LOCK_MACOS) {
      flags |= O_EXLOCK;
   }
#elif !defined(__FreeBSD__) && !defined(sun) && !defined(N_PLAT_NLM)
   /*
    * If FILEIO_OPEN_EXCLUSIVE_LOCK or FILEIO_OPEN_MULTIWRITER_LOCK or
    * (FILEIO_OPEN_ACCESS_READ | FILEIO_OPEN_LOCKED) are passed, and we are
    * on VMFS, then pass in special flags to get exclusive, multiwriter, or
    * cross-host read-only mode.  The first if statement is to avoid calling
    * HostType_OSIsVMK() unless really necessary.
    *
    * If the above conditions are met FILEIO_OPEN_LOCKED, is filtered out --
    * vmfs will be handling the locking, so there is no need to create
    * lockfiles.
    */
   if ((access & (FILEIO_OPEN_EXCLUSIVE_LOCK |
                  FILEIO_OPEN_MULTIWRITER_LOCK)) != 0 ||
       (access & (FILEIO_OPEN_ACCESS_READ | FILEIO_OPEN_ACCESS_WRITE |
                  FILEIO_OPEN_LOCKED)) ==
       (FILEIO_OPEN_ACCESS_READ | FILEIO_OPEN_LOCKED)) {
      if (HostType_OSIsVMK()) {
         access &= ~FILEIO_OPEN_LOCKED;

         if ((access & FILEIO_OPEN_MULTIWRITER_LOCK) != 0) {
            flags |= O_MULTIWRITER_LOCK;
         } else {
            flags |= O_EXCLUSIVE_LOCK;
         }
      }
   }
#endif

   FileIO_Init(file, pathName);
   ret = FileIO_Lock(file, access);
   if (!FileIO_IsSuccess(ret)) {
      goto error;
   }

   if ((access & (FILEIO_OPEN_ACCESS_READ | FILEIO_OPEN_ACCESS_WRITE)) ==
       (FILEIO_OPEN_ACCESS_READ | FILEIO_OPEN_ACCESS_WRITE)) {
      flags |= O_RDWR;
   } else if (access & FILEIO_OPEN_ACCESS_WRITE) {
      flags |= O_WRONLY;
   } else if (access & FILEIO_OPEN_ACCESS_READ) {
      flags |= O_RDONLY;
   }

   if (access & FILEIO_OPEN_EXCLUSIVE_READ &&
       access & FILEIO_OPEN_EXCLUSIVE_WRITE) {
      flags |= O_EXCL;
   }

   if (access & FILEIO_OPEN_UNBUFFERED) {
#if defined(FILEIO_SUPPORT_ODIRECT)
      flags |= O_DIRECT;
#elif !defined(__APPLE__) // Mac hosts need this access flag after opening.
      access &= ~FILEIO_OPEN_UNBUFFERED;
      LOG_ONCE((LGPFX" %s reverting to buffered IO on %s.\n",
                __FUNCTION__, UTF8(pathName)));
#endif
   }

   if (access & FILEIO_OPEN_NONBLOCK) {
      flags |= O_NONBLOCK;
   }

   file->flags = access;

   if (access & FILEIO_OPEN_PRIVILEGED) {
      uid = Id_BeginSuperUser();
   }

   flags |= 
#if defined(linux) && !defined(N_PLAT_NLM)
            ((access & FILEIO_OPEN_SYNC) ? O_SYNC : 0) |
#endif
            FileIO_OpenActions[action];

   fd = PosixFileOpener(pathName, flags, mode);

   error = errno;

   if (access & FILEIO_OPEN_PRIVILEGED) {
      Id_EndSuperUser(uid);
   }

   errno = error;

   if (fd == -1) {
      ret = FileIOErrno2Result(errno);
      goto error;
   }

#if defined(__APPLE__)
   if (access & (FILEIO_OPEN_UNBUFFERED | FILEIO_OPEN_SYNC)) {
      error = fcntl(fd, F_NOCACHE, 1);
      if (error == -1) {
         ret = FileIOErrno2Result(errno);
         goto error;
      }
   }
#endif

   if (access & FILEIO_OPEN_DELETE_ASAP) {
      /*
       * Remove the name from the name space. The file remains laid out on the
       * disk and accessible through the file descriptor until it is closed.
       */

      if (Posix_Unlink(pathName) == -1) {
         ret = FileIOErrno2Result(errno);
         goto error;
      }
   }

   file->posix = fd;

   return FILEIO_SUCCESS;

error:
   error = errno;

   if (fd != -1) {
      close(fd);
   }
   FileIO_Unlock(file);
   FileIO_Cleanup(file);
   FileIO_Invalidate(file);
   errno = error;

   return ret;
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_Open --
 *
 *      Open/create a file
 *
 * Results:
 *      FILEIO_SUCCESS on success: 'file' is set
 *      FILEIO_OPEN_ERROR_EXIST if the file already exists
 *      FILEIO_FILE_NOT_FOUND if the file is not present
 *      FILEIO_ERROR for other errors
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

FileIOResult
FileIO_Open(FileIODescriptor *file,   // OUT:
            ConstUnicode pathName,    // IN:
            int access,               // IN:
            FileIOOpenAction action)  // IN:
{
   return FileIO_Create(file, pathName, access, action, S_IRUSR | S_IWUSR);
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_Seek --
 *
 *      Change the current position in a file
 *
 * Results:
 *      On success: the new current position in bytes from the beginning of
 *                  the file
 *
 *      On failure: -1
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

#if defined(linux) && !defined(N_PLAT_NLM) && defined(SYS__llseek)
/*
 * If the llseek system call exists, use it to provide a version of 64-bit
 * lseek() functionality, for FileIO_Seek() to use if appropriate.
 */

#define VM_HAVE__LLSEEK 1

static INLINE int
_llseek(unsigned int fd,
        unsigned long offset_high,
        unsigned long offset_low,
        loff_t * result,
        unsigned int whence)
{
   return syscall(SYS__llseek, fd, offset_high, offset_low, result, whence);
}
#endif

uint64
FileIO_Seek(const FileIODescriptor *file,  // IN:
            int64 distance,                // IN:
            FileIOSeekOrigin origin)       // IN:
{
   ASSERT(file);

   /*
    * The goal is to use the best lseek-type function with support for 64-bit
    * file offsets (aka large file support, or LFS).
    *
    * __USE_LARGEFILE implies that lseek() has LFS
    * VM_HAVE__LLSEEK tells us that we have the _llseek() routine available
    * __USE_LARGEFILE64 implies that lseek64() is available
    *
    * All three of these defines only come into play on Linux systems. On any
    * other OS, they won't be present, and we go straight for lseek() since
    * that's the only known alternative.
    */
#if defined(VM_HAVE__LLSEEK) && !defined(__USE_LARGEFILE) && !defined(__USE_LARGEFILE64)
   /*
    * This is a Linux system that doesn't have a glibc with any large-file
    * support (LFS), but does have the llseek system call. On Linux, this is
    * the least desirable option because the API is a bit grotty (e.g. the
    * casting of negative offsets into unsigned offset_hi and offset_lo), and
    * because doing system calls directly from our code is more likely to
    * break than using libc.
    */

   loff_t res;

   if (_llseek(file->posix, distance >> 32, distance & 0xFFFFFFFF,
               &res, FileIO_SeekOrigins[origin]) == -1) {
      res = -1;
   }

   return res;
#elif defined(__USE_LARGEFILE64) && !defined(__USE_LARGEFILE)
   /*
    * This is a Linux system with glibc that has lseek64 available, but not a
    * lseek with LFS.
    *
    * lseek64 is a bit cleaner than _llseek (because glibc provides it, we
    * know the API won't break) but still not as portable as plain old lseek.
    */

    return lseek64(file->posix, distance, FileIO_SeekOrigins[origin]);
#else
    /*
     * We're taking this route because either we know lseek() can support
     * 64-bit file offsets (__USE_LARGEFILE is set) or because llseek and
     * lseek64 are both unavailable.
     *
     * This means this a Linux/glibc system with transparent LFS support, an
     * old Linux system without llseek, or another POSIX system.
     */

    return lseek(file->posix, distance, FileIO_SeekOrigins[origin]);
#endif
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_Write --
 *
 *      Write to a file
 *
 * Results:
 *      FILEIO_SUCCESS on success: '*actual_count' = 'requested' bytes have
 *       been written
 *      FILEIO_WRITE_ERROR_FBIG for the attempt to write file that exceeds
 *       maximum file size
 *      FILEIO_WRITE_ERROR_NOSPC when the device containing the file has no
 *       room for the data
 *      FILEIO_WRITE_ERROR_DQUOT for attempts to write file that exceeds
 *       user's disk quota 
 *      FILEIO_ERROR for other errors: only '*actual_count' bytes have been
 *       written for sure
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

FileIOResult
FileIO_Write(FileIODescriptor *fd,  // IN:
             const void *bufIn,     // IN:
             size_t requested,      // IN:
             size_t *actual)        // OUT:
{
   const uint8 *buf = (const uint8 *)bufIn;
   size_t initial_requested;
   FileIOResult fret = FILEIO_SUCCESS;

   ASSERT(fd);

   ASSERT_NOT_IMPLEMENTED(requested < 0x80000000);

   initial_requested = requested;
   while (requested > 0) {
      ssize_t res;

      res = write(fd->posix, buf, requested);

      if (res == -1) {
         int error = errno;

         if (error == EINTR) {
            NOT_TESTED();
            continue;
         }
         fret = FileIOErrno2Result(error);
         break;
      }

      buf += res;
      requested -= res;
   }

   if (actual) {
      *actual = initial_requested - requested;
   }
   return fret;
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_Read --
 *
 *      Read from a file
 *
 * Results:
 *      FILEIO_SUCCESS on success: '*actual_count' = 'requested' bytes have
 *       been written
 *      FILEIO_READ_ERROR_EOF if the end of the file was reached: only
 *       '*actual_count' bytes have been read for sure
 *      FILEIO_ERROR for other errors: only '*actual_count' bytes have been
 *       read for sure
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

FileIOResult
FileIO_Read(FileIODescriptor *fd,  // IN:
            void *bufIn,           // OUT:
            size_t requested,      // IN:
            size_t *actual)        // OUT:
{
   uint8 *buf = (uint8 *) bufIn;
   size_t initial_requested;
   FileIOResult fret = FILEIO_SUCCESS;

   ASSERT(fd);

   ASSERT_NOT_IMPLEMENTED(requested < 0x80000000);

   initial_requested = requested;
   while (requested > 0) {
      ssize_t res;

      res = read(fd->posix, buf, requested);
      if (res == -1) {
         if (errno == EINTR) {
            NOT_TESTED();
            continue;
         }
         fret = FileIOErrno2Result(errno);
         if (FILEIO_ERROR == fret) {
            Log("read failed, errno=%d, %s\n", errno, Err_Errno2String(errno));
         }
         break;
      }

      if (res == 0) {
         fret = FILEIO_READ_ERROR_EOF;
         break;
      }

      buf += res;
      requested -= res;
   }

   if (actual) {
      *actual = initial_requested - requested;
   }
   return fret;
}


#if !defined(N_PLAT_NLM)
/*
 *----------------------------------------------------------------------
 *
 * FileIO_Truncate --
 *
 *      Truncates file to a given length
 *
 * Results:
 *      Bool - TRUE on success, FALSE on failure
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

Bool
FileIO_Truncate(FileIODescriptor *file,  // IN:
                uint64 newLength)        // IN:
{
   ASSERT(file);

   return ftruncate(file->posix, newLength) == 0;
}
#endif /* !defined(N_PLAT_NLM) */


/*
 *----------------------------------------------------------------------
 *
 * FileIO_Close --
 *
 *      Close a file
 *
 * Results:
 *      TRUE: an error occured
 *      FALSE: no error occured
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

Bool
FileIO_Close(FileIODescriptor *file)  // IN:
{
   int err;

   ASSERT(file);

   err = (close(file->posix) == -1) ? errno : 0;

   /* Unlock the file if it was locked */
   FileIO_Unlock(file);
   FileIO_Cleanup(file);
   FileIO_Invalidate(file);

   if (err) {
      errno = err;
   }

   return err != 0;
}


#if !defined(N_PLAT_NLM)
/*
 *----------------------------------------------------------------------
 *
 * FileIO_Sync --
 *
 *      Synchronize the disk state of a file with its memory state
 *
 * Results:
 *      On success: 0
 *      On failure: -1
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

int
FileIO_Sync(const FileIODescriptor *file)  // IN:
{
   ASSERT(file);

   return fsync(file->posix);
}


/*
 *-----------------------------------------------------------------------------
 *
 * FileIOCoalesce --
 *
 *      Linux 2.2 does a fairly braindead thing with ioVec's.  It simply issues
 *      reads and writes internal to the kernel in serial
 *      (linux/fs/read_write.c:do_readv_writev()).  We optimize here for the
 *      case of many small chunks.  The cost of the extra copy in this case
 *      is made up for by the decreased number of separate I/Os the kernel
 *      issues internally. Note that linux 2.4 seems to be smarter with respect
 *      to this problem.
 *
 * Results:
 *      Bool - Whether or not coalescing was done.  If it was done,
 *             FileIODecoalesce *MUST* be called.
 *
 * Side effects:
 *      FileIOCoalesce will malloc *outVec if coalescing is performed
 *
 *-----------------------------------------------------------------------------
 */

static Bool
FileIOCoalesce(struct iovec *inVec,   // IN:  Vector to coalesce from
               int inCount,           // IN:  count for inVec
               size_t inTotalSize,    // IN:  totalSize (bytes) in inVec
               Bool isWrite,          // IN:  coalesce for writing (or reading)
               Bool forceCoalesce,    // IN:  if TRUE always coalesce
               int flags,             // IN: fileIO open flags
               struct iovec *outVec)  // OUT: Coalesced (1-entry) iovec
{
   uint8 *cBuf;

   ASSERT(inVec);
   ASSERT(outVec);

   FileIO_OptionalSafeInitialize();

   /* simple case: no need to coalesce */
   if (inCount == 1) {
      return FALSE;
   }

   /*
    * Only coalesce when the number of entries is above our count threshold
    * and the average size of an entry is less than our size threshold
    */

   if (!forceCoalesce &&
       (!filePosixOptions.enabled ||
       inCount <= filePosixOptions.countThreshold ||
       inTotalSize / inCount >= filePosixOptions.sizeThreshold)) {
      return FALSE;
   }

   // XXX: Wouldn't it be nice if we could log from here!
   //LOG(5, ("FILE: Coalescing %s of %d elements and %d size\n",
   //        isWrite ? "write" : "read", inCount, inTotalSize));

   if (filePosixOptions.aligned || flags & FILEIO_OPEN_UNBUFFERED) {
      cBuf = Aligned_Malloc(sizeof(uint8) * inTotalSize);
   } else {
      cBuf = Util_SafeMalloc(sizeof(uint8) * inTotalSize);
   }
   if (!cBuf) {
      return FALSE;
   }

  if (isWrite) {
      IOV_WriteIovToBuf(inVec, inCount, cBuf, inTotalSize);
   }

   outVec->iov_base = cBuf;
   outVec->iov_len = inTotalSize;

   return TRUE;
}


/*
 *-----------------------------------------------------------------------------
 *
 * FileIODecoalesce --
 *
 *      Inverse of the coalesce optimization.  For writes, its a NOOP, but
 *      for reads, it copies the data back into the original buffer.
 *      It also frees the memory allocated by FileIOCoalesce.
 *
 * Results:
 *      void
 *
 * Side effects:
 *      None
 *
 *-----------------------------------------------------------------------------
 */

static void
FileIODecoalesce(struct iovec *coVec,   // IN: Coalesced (1-entry) vector
                 struct iovec *origVec, // IN: Original vector
                 int origVecCount,      // IN: count for origVec
                 size_t actualSize,     // IN: # bytes to transfer back to origVec
                 Bool isWrite,          // IN: decoalesce for writing (or reading)
                 int flags)             // IN: fileIO open flags
{
   ASSERT(coVec);
   ASSERT(origVec);

   ASSERT(actualSize <= coVec->iov_len);
   ASSERT_NOT_TESTED(actualSize == coVec->iov_len);

   if (!isWrite) {
      IOV_WriteBufToIov(coVec->iov_base, actualSize, origVec, origVecCount);
   }

   if (filePosixOptions.aligned || flags & FILEIO_OPEN_UNBUFFERED) {
      Aligned_Free(coVec->iov_base);
   } else {
      free(coVec->iov_base);
   }
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_Readv --
 *
 *      Wrapper for readv. In linux, we can issue a readv directly.
 *      But the readv is not atomic, i.e, the read can succeed
 *      on the first N vectors, and return a positive value in spite
 *      of the fact that there was an error on the N+1st vector. There
 *      is no way to query the exact error that happened. So, we retry
 *      in a loop (for a max of MAX_RWV_RETRIES).
 *      XXX: If we retried MAX_RWV_RETRIES times and gave up, we will
 *      return FILEIO_ERROR even if errno is undefined.
 *
 * Results:
 *      FILEIO_SUCCESS, FILEIO_ERROR, FILEIO_READ_ERROR_EOF
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

FileIOResult
FileIO_Readv(FileIODescriptor *fd,  // IN:
             struct iovec *v,       // IN:
             int numEntries,        // IN:
             size_t totalSize,      // IN:
             size_t *actual)        // OUT:
{
   size_t bytesRead = 0, sum = 0;
   FileIOResult fret = FILEIO_ERROR;
   int nRetries = 0, maxRetries = numEntries;
   struct iovec coV;
   struct iovec *vPtr;
   Bool didCoalesce;
   int numVec;

   ASSERT(fd);

   didCoalesce = FileIOCoalesce(v, numEntries, totalSize, FALSE,
                                FALSE, fd->flags, &coV);

   ASSERT_NOT_IMPLEMENTED(totalSize < 0x80000000);

   numVec = didCoalesce ? 1 : numEntries;
   vPtr = didCoalesce ? &coV : v;

   while (nRetries < maxRetries) {
      ssize_t retval;

      ASSERT(numVec > 0);
      retval = readv(fd->posix, vPtr, numVec);

      if (retval == -1) {
         if (errno == EINTR) {
            NOT_TESTED();
            continue;
         }
         fret = FileIOErrno2Result(errno);
         break;
      }
      bytesRead += retval;
      if (bytesRead == totalSize) {
         fret =  FILEIO_SUCCESS;
         break;
      }
      if (retval == 0) {
         fret = FILEIO_READ_ERROR_EOF;
         break;
      }

      /*
       * Ambigous case. Stupid Linux. If the bytesRead matches an
       * exact iovector boundary, we need to retry from the next
       * iovec. 2) If it does not match, EOF is the only error possible.
       * NOTE: If Linux Readv implementation changes, this
       * ambiguity handling may need to change.
       * --Ganesh, 08/15/2001.
       */

      for (; sum <= bytesRead; vPtr++, numVec--) {
         sum += vPtr->iov_len;

         /*
          * In each syscall, we will process atleast one iovec
          * or get an error back. We will therefore retry atmost
          * count times. If multiple iovecs were processed before
          * an error hit, we will retry a lesser number of times.
          */

         nRetries++;
      }
      if (sum > bytesRead) {
         // A partially filled iovec can ONLY mean EOF
         fret = FILEIO_READ_ERROR_EOF;
         break;
      }
   }

   if (didCoalesce) {
      FileIODecoalesce(&coV, v, numEntries, bytesRead, FALSE, fd->flags);
   }

   if (actual) {
      *actual = bytesRead;
   }

   return fret;
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_Writev --
 *
 *      Wrapper for writev. In linux, we can issue a writev directly.
 *      But the writev is not atomic, i.e, the write can succeed
 *      on the first N vectors, and return a positive value in spite
 *      of the fact that there was an error on the N+1st vector. There
 *      is no way to query the exact error that happened. So, we retry
 *      in a loop (for a max of MAX_RWV_RETRIES).
 *      XXX: If we retried MAX_RWV_RETRIES times and gave up, we will
 *      return FILEIO_ERROR even if errno is undefined.
 *
 * Results:
 *      FILEIO_SUCCESS, FILEIO_ERROR
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

FileIOResult
FileIO_Writev(FileIODescriptor *fd,  // IN:
              struct iovec *v,       // IN:
              int numEntries,        // IN:
              size_t totalSize,      // IN:
              size_t *actual)        // OUT:
{
   size_t bytesWritten = 0, sum = 0;
   FileIOResult fret = FILEIO_ERROR;
   int nRetries = 0, maxRetries = numEntries;
   struct iovec coV;
   struct iovec *vPtr;
   Bool didCoalesce;
   int numVec;

   ASSERT(fd);

   didCoalesce = FileIOCoalesce(v, numEntries, totalSize, TRUE,
                                FALSE, fd->flags, &coV);

   ASSERT_NOT_IMPLEMENTED(totalSize < 0x80000000);

   numVec = didCoalesce ? 1 : numEntries;
   vPtr = didCoalesce ? &coV : v;

   while (nRetries < maxRetries) {
      ssize_t retval;

      ASSERT(numVec > 0);
      retval = writev(fd->posix, vPtr, numVec);

      if (retval == -1) {
         fret = FileIOErrno2Result(errno);
         break;
      }

      bytesWritten += retval;
      if (bytesWritten == totalSize) {
         fret =  FILEIO_SUCCESS;
         break;
      }
      NOT_TESTED();
      for (; sum <= bytesWritten; vPtr++, numVec--) {
         sum += vPtr->iov_len;
         nRetries++;
      }

      /*
       * writev only seems to produce a partial iovec when the disk is
       * out of space.  Just call it an error. --probin
       */

      if (sum != bytesWritten) {
         fret = FILEIO_WRITE_ERROR_NOSPC;
         break;
      }
   }

   if (didCoalesce) {
      FileIODecoalesce(&coV, v, numEntries, bytesWritten, TRUE, fd->flags);
   }

   if (actual) {
      *actual = bytesWritten;
   }

   return fret;
}


#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) ||\
    defined(sun)

/*
 *----------------------------------------------------------------------
 *
 * FileIO_Preadv --
 *
 *      Implementation of vector pread. The incoming vectors are
 *      coalesced to a single buffer to issue only one pread()
 *      system call which reads from a specified offset. The
 *      vectors are then decoalesced before return.
 *
 * Results:
 *      FILEIO_SUCCESS, FILEIO_ERROR
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

FileIOResult
FileIO_Preadv(FileIODescriptor *fd,   // IN: File descriptor
              struct iovec *entries,  // IN: Vector to read into
              int numEntries,         // IN: Number of vector entries
              uint64 offset,          // IN: Offset to start reading
              size_t totalSize)       // IN: totalSize (bytes) in entries
{
   size_t sum = 0;
   struct iovec *vPtr;
   struct iovec coV;
   int count;
   uint64 fileOffset;
   FileIOResult fret;
   Bool didCoalesce;

   ASSERT(fd);
   ASSERT(entries);
   ASSERT(!(fd->flags & FILEIO_ASYNCHRONOUS));
   ASSERT_NOT_IMPLEMENTED(totalSize < 0x80000000);

   didCoalesce = FileIOCoalesce(entries, numEntries, totalSize, FALSE,
                                TRUE /* force coalescing */, fd->flags, &coV);

   count = didCoalesce ? 1 : numEntries;
   vPtr = didCoalesce ? &coV : entries;

   fileOffset = offset;
   while (count > 0) {
      size_t leftToRead = vPtr->iov_len;
      uint8 *buf = (uint8 *) vPtr->iov_base;

      while (leftToRead > 0) {
         ssize_t retval = pread(fd->posix, buf, leftToRead, fileOffset);

         if (retval == -1) {
            if (errno == EINTR) {
               LOG_ONCE((LGPFX" %s got EINTR.  Retrying\n", __FUNCTION__));
               NOT_TESTED_ONCE();
               continue;
            }
            fret = FileIOErrno2Result(errno);
            goto exit;
         }

         if (retval == 0) {
            fret = FILEIO_READ_ERROR_EOF;
            goto exit;
         }

         buf += retval;
         leftToRead -= retval;
         sum += retval;
         fileOffset += retval;
      }

      count--;
      vPtr++;
   }
   fret = FILEIO_SUCCESS;

exit:
   if (didCoalesce) {
      FileIODecoalesce(&coV, entries, numEntries, sum, FALSE, fd->flags);
   }

   return fret;
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_Pwritev --
 *
 *      Implementation of vector pwrite. The incoming vectors are
 *      coalesced to a single buffer to issue only one pwrite()
 *      system call which writes from a specified offset. The
 *      vectors are then decoalesced before return.
 *
 * Results:
 *      FILEIO_SUCCESS, FILEIO_ERROR
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

FileIOResult
FileIO_Pwritev(FileIODescriptor *fd,   // IN: File descriptor
               struct iovec *entries,  // IN: Vector to write from
               int numEntries,         // IN: Number of vector entries
               uint64 offset,          // IN: Offset to start writing
               size_t totalSize)       // IN: Total size (bytes) in entries
{
   struct iovec coV;
   Bool didCoalesce;
   struct iovec *vPtr;
   int count;
   size_t sum = 0;
   uint64 fileOffset;
   FileIOResult fret;

   ASSERT(fd);
   ASSERT(entries);
   ASSERT(!(fd->flags & FILEIO_ASYNCHRONOUS));
   ASSERT_NOT_IMPLEMENTED(totalSize < 0x80000000);

   didCoalesce = FileIOCoalesce(entries, numEntries, totalSize, TRUE,
                                TRUE /* force coalescing */, fd->flags, &coV);

   count = didCoalesce ? 1 : numEntries;
   vPtr = didCoalesce ? &coV : entries;

   fileOffset = offset;
   while (count > 0) {
      size_t leftToWrite = vPtr->iov_len;
      uint8 *buf = (uint8 *)vPtr->iov_base;

      while (leftToWrite > 0) {
         ssize_t retval = pwrite(fd->posix, buf, leftToWrite, fileOffset);

         if (retval == -1) {
            if (errno == EINTR) {
               LOG_ONCE((LGPFX" %s got EINTR.  Retrying\n", __FUNCTION__));
               NOT_TESTED_ONCE();
               continue;
            }
            fret = FileIOErrno2Result(errno);
            goto exit;
         }
         if (retval == 0) {
            NOT_TESTED();
            fret = FILEIO_WRITE_ERROR_NOSPC;
            goto exit;
         }
         if (retval < leftToWrite) {
            LOG_ONCE((LGPFX" %s wrote %"FMTSZ"d out of %"FMTSZ"u bytes.\n",
                      __FUNCTION__, retval, leftToWrite));
         }

         buf += retval;
         leftToWrite -= retval;
         sum += retval;
         fileOffset += retval;
      }

      count--;
      vPtr++;
   }

   fret = FILEIO_SUCCESS;
exit:
   if (didCoalesce) {
      FileIODecoalesce(&coV, entries, numEntries, sum, TRUE, fd->flags);
   }

   return fret;
}
#endif /* defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) ||
          defined(sun) */
#endif /* !defined(N_PLAT_NLM) */

/*
 *----------------------------------------------------------------------
 *
 * FileIO_GetSize --
 *
 *      Get size of file.
 *
 * Results:
 *      Size of file or -1.
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

int64
FileIO_GetSize(const FileIODescriptor *fd)  // IN:
{
   struct stat statBuf;

   ASSERT(fd);

   return (fstat(fd->posix, &statBuf) == -1) ? -1 : statBuf.st_size;
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_GetAllocSize --
 *
 *      Get allocated size of file.
 *
 * Results:
 *      Size of file or -1.
 *
 * Side effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

int64
FileIO_GetAllocSize(const FileIODescriptor *fd)  // IN
{
   struct stat s;

   ASSERT(fd);

#if __linux__ && defined(N_PLAT_NLM)
   /* Netware doesn't have st_blocks.  Just fall back to GetSize. */
   return FileIO_GetSize(fd);
#else
   /*
    * If you don't like the magic number 512, yell at the people
    * who wrote sys/stat.h and tell them to add a #define for it.
    */
   return (fstat(fd->posix, &s) == -1) ? -1 : s.st_blocks * 512;
#endif
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_SetAllocSize --
 *
 *      Set allocated size of file, allocating new blocks if needed.
 *      It is an error for size to be less than the current size.
 *
 * Results:
 *      TRUE on success.  Sets errno on failure.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Bool
FileIO_SetAllocSize(const FileIODescriptor *fd,  // IN:
                    uint64 size)                 // IN:
{

#if defined(__APPLE__) || defined(__linux__)
   uint64 curSize;
   uint64 preallocLen;
#if defined(__APPLE__)
   fstore_t prealloc;
#endif

   curSize = FileIO_GetAllocSize(fd);
   if (curSize > size) {
      errno = EINVAL;

      return FALSE;
   }
   preallocLen = size - curSize;

#if defined(__APPLE__)
   prealloc.fst_flags = 0;
   prealloc.fst_posmode = F_PEOFPOSMODE;
   prealloc.fst_offset = 0;
   prealloc.fst_length = preallocLen;
   prealloc.fst_bytesalloc = 0;

   return fcntl(fd->posix, F_PREALLOCATE, &prealloc) != -1;
#elif __linux__
   return syscall(SYS_fallocate, fd->posix, FALLOC_FL_KEEP_SIZE,
                  curSize, preallocLen) == 0;
#endif

#else
   errno = ENOSYS;

   return FALSE;
#endif
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_GetSizeByPath --
 *
 *      Get size of a file specified by path. 
 *
 * Results:
 *      Size of file or -1.
 *
 * Side effects:
 *      errno is set on error
 *
 *----------------------------------------------------------------------
 */

int64
FileIO_GetSizeByPath(ConstUnicode pathName)  // IN:
{
   int err;
   struct stat statbuf;

   err = Posix_Stat(pathName, &statbuf);

   return (err == 0) ? statbuf.st_size : -1;
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_Access --
 *
 *      Wrapper for access syscall. We return FILEIO_SUCCESS if the file
 *      is accessible with the specified mode. If not, we will return
 *      FILEIO_ERROR.
 *
 * Results:
 *      FILEIO_SUCCESS or FILEIO_ERROR.
 *
 * Side effects:
 *      errno is set on error
 *
 *----------------------------------------------------------------------
 */

FileIOResult
FileIO_Access(ConstUnicode pathName,  // IN: Path name. May be NULL.
              int accessMode)         // IN: Access modes to be asserted
{
   int mode = 0;

   if (pathName == NULL) {
      errno = EFAULT;

      return FILEIO_ERROR;
   }

   if (accessMode & FILEIO_ACCESS_READ) {
      mode |= R_OK;
   }
   if (accessMode & FILEIO_ACCESS_WRITE) {
      mode |= W_OK;
   }
   if (accessMode & FILEIO_ACCESS_EXEC) {
      mode |= X_OK;
   }
   if (accessMode & FILEIO_ACCESS_EXISTS) {
      mode |= F_OK;
   }

   return (Posix_Access(pathName, mode) == -1) ? FILEIO_ERROR : FILEIO_SUCCESS;
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_GetFlags --
 *
 *      Accessor for fd->flags;
 *
 * Results:
 *      fd->flags
 *
 * Side Effects:
 *      None
 *
 *----------------------------------------------------------------------
 */

uint32
FileIO_GetFlags(FileIODescriptor *fd)  // IN:
{
   ASSERT(fd);
   ASSERT(FileIO_IsValid(fd));

   return fd->flags;
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_SupportsFileSize --
 *
 *      Test whether underlying filesystem supports specified file size.
 *
 * Results:
 *      Return TRUE if such file size is supported, FALSE otherwise.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Bool
FileIO_SupportsFileSize(const FileIODescriptor *fd,  // IN:
                        uint64 requestedSize)        // IN:
{
#if defined(N_PLAT_NLM)
   /* API we use on NetWare cannot go over 2GB - 1 although filesystem could. */
   return requestedSize <= 0x7FFFFFFF;
#elif defined(linux)
   /*
    * Linux makes test on seek(), so we can do simple non-intrusive test.
    * Verified to work on 2.2.x, 2.4.x and 2.6.x, with ext2, ext3, smbfs, 
    * cifs, nfs and ncpfs.  Always got some reasonable value.
    */
   Bool supported = FALSE;
   uint64 oldPos;

   ASSERT(FileIO_IsValid(fd));

   oldPos = FileIO_Seek(fd, 0, FILEIO_SEEK_CURRENT);
   if (oldPos != (uint64)-1) {
      uint64 newPos;

      if (FileIO_Seek(fd, requestedSize, FILEIO_SEEK_BEGIN) == requestedSize) {
         supported = TRUE;
      }
      newPos = FileIO_Seek(fd, oldPos, FILEIO_SEEK_BEGIN);
      ASSERT_NOT_IMPLEMENTED(oldPos == newPos);
   }

   return supported;
#elif defined(__APPLE__)
   struct statfs buf;

   if (fstatfs(fd->posix, &buf) == -1) {
      Log(LGPFX" %s fstatfs failure: %s\n", __FUNCTION__,
          Err_Errno2String(errno));
      /* Be optimistic despite failure */
      return TRUE;
   }

   /* Check for FAT and UFS file systems */
   if ((Str_Strcasecmp(buf.f_fstypename, "msdos") == 0) ||
       (Str_Strcasecmp(buf.f_fstypename, "ufs") == 0)) {
      /* 4 GB limit */
      return requestedSize > CONST64U(0xFFFFFFFF) ? FALSE : TRUE;
   }

   /* Be optimistic... */
   return TRUE;
#else
   /* Be optimistic on FreeBSD and Solaris... */
   return TRUE;
#endif
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_GetModTime --
 *
 *      Retrieve last modification time.
 *
 * Results:
 *      Return POSIX epoch time or -1 on error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int64
FileIO_GetModTime(const FileIODescriptor *fd)  // IN:
{
   struct stat statbuf;

   if (fstat(fd->posix, &statbuf) == 0) {
      return statbuf.st_mtime;
   } else {
      return -1;
   }
}


/*
 *----------------------------------------------------------------------
 *
 * FileIO_PrivilegedPosixOpen --
 *
 *      Opens file with elevated privileges.
 *
 * Results:
 *      Opened file descriptor, or -1 on failure (errno contains error code).
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int
FileIO_PrivilegedPosixOpen(ConstUnicode pathName,  // IN:
                           int flags)              // IN:
{
   int fd;
   Bool suDone;
   uid_t uid = -1;

   if (pathName == NULL) {
      errno = EFAULT;

      return -1;
   }

   /*
    * I've said *opens*.  I want you really think twice before creating files
    * with elevated privileges, so for them you have to use Id_BeginSuperUser()
    * yourself.
    */

   ASSERT((flags & (O_CREAT | O_TRUNC)) == 0);

   if (Id_IsSuperUser()) {
      suDone = FALSE;
   } else {
      uid = Id_BeginSuperUser();
      suDone = TRUE;
   }

   fd = Posix_Open(pathName, flags, 0);

   if (suDone) {
      int error = errno;

      Id_EndSuperUser(uid);
      errno = error;
   }


   return fd;
}


/*
 *-----------------------------------------------------------------------------
 *
 * FileIO_DescriptorToStream
 *
 *      Return a FILE * stream equivalent to the given FileIODescriptor.
 *      This is the logical equivalent of Posix dup() then fdopen().
 *
 *      Caller should fclose the returned descriptor when finished.
 *
 * Results:
 *      !NULL  A FILE * associated with the file associated with the fd
 *      NULL   Failure
 *
 * Side effects:
 *      New fd allocated.
 *
 *-----------------------------------------------------------------------------
 */

FILE *
FileIO_DescriptorToStream(FileIODescriptor *fdesc,  // IN:
                          Bool textMode)            // IN: unused
{
   int dupFD;
   const char *mode;
   int tmpFlags;
   FILE *stream;

   dupFD = dup(fdesc->posix);
   if (dupFD == -1) {
      return NULL;
   }

   /* The file you pass us should be valid and opened for *something* */
   ASSERT(FileIO_IsValid(fdesc));
   ASSERT((fdesc->flags & (FILEIO_OPEN_ACCESS_READ | FILEIO_OPEN_ACCESS_WRITE)) != 0);
   tmpFlags = fdesc->flags & (FILEIO_OPEN_ACCESS_READ | FILEIO_OPEN_ACCESS_WRITE);

   if (tmpFlags == (FILEIO_OPEN_ACCESS_READ | FILEIO_OPEN_ACCESS_WRITE)) {
      mode = "r+";
   } else if (tmpFlags == FILEIO_OPEN_ACCESS_WRITE) {
      mode = "w";
   } else {  /* therefore (tmpFlags == FILEIO_OPEN_ACCESS_READ) */
      mode = "r";
   }

   stream = fdopen(dupFD, mode);

   if (stream == NULL) {
      close(dupFD);
   }

   return stream;
}


#if defined(__APPLE__)
/*
 *----------------------------------------------------------------------
 *
 * HostSupportsPrealloc --
 *
 *      Returns TRUE if the host OS is new enough to support F_PREALLOCATE
 *      without data loss bugs.  On OSX, this has been verified fixed
 *      on 10.6 build with kern.osreleasae 10.0.0d6.
 *
 * Results:
 *      TRUE if the current host OS is new enough.
 *      FALSE if it is not or we can't tell because of an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

static Bool
HostSupportsPrealloc(void)
{
   char curRel[32];
   char type;
   unsigned static const int req[] = { 10, 0, 0, 6 };
   unsigned int cur[4], i;
   int num;
   size_t len = sizeof(curRel);
   Bool ret = FALSE;

   if (sysctlbyname("kern.osrelease", (void *) &curRel, &len, NULL, 0) == -1) {
      goto exit;
   }

   curRel[31] = '\0';
   Log("Current OS Release is %s\n", curRel);

   /*
    * Apple's osversion is in the format X.Y.Z which maps to the public
    * OSX version 10.X-4.Y, and Z is incremented for each publicly
    * released build.  The Z part is of the form A<type>B, where a and
    * B are version numbers and <type> is either d (devel), a (alpha),
    * b (beta), rc, or fc.  If the <type>B is missing, then its a GA build.
    *
    * Since we're checking for 10.0.0d6, we can just say anything without
    * a type or with a type other than d is higher.  For d, we compare
    * the last number.
    */

   num = sscanf(curRel, "%u.%u.%u%c%u", &cur[0], &cur[1], &cur[2], &type,
                &cur[3]);

   if (num < 3) {
      goto exit;
   }

   for (i = 0; i < 3; i++) {
      if (req[i] > cur[i]) {
         goto exit;
      } else if (req[i] < cur[i]) {
         ret = TRUE;
         goto exit; 
      }
   }
   if (num == 5 && type == 'd') {
      ret = req[3] <= cur[3];
      goto exit;
   }

   /*
    * If we get a type with no letter (num == 4), thats odd.
    * Consider it mal-formatted and fail.
    */

   ret = num != 4;

exit:
   if (!ret && filePosixOptions.initialized && 
       filePosixOptions.aioNumThreads == 1) {
      ret =TRUE;
   }

   return  ret;
}

#else

/*
 *----------------------------------------------------------------------
 *
 * HostSupportsPrealloc --
 *
 *      fallocate() is supported for ext4 and xfs since 2.6.23 kernels
 *
 * Results:
 *      TRUE if the current host is linux and kernel is >= 2.6.23.
 *      FALSE if it is not .
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

static Bool
HostSupportsPrealloc(void)
{
#if  (defined(__linux__ ) && !defined(VMX86_SERVER))
    if (Hostinfo_OSVersion(0) >=2 && Hostinfo_OSVersion(1) >=6 &&
        Hostinfo_OSVersion(2) >=23) {
       return TRUE;
    }
#endif
    return FALSE;
}

#endif


/*
 *----------------------------------------------------------------------------
 *
 * FileIO_SupportsPrealloc --
 *
 *      Checks if the HostOS/filesystem supports preallocation.
 *
 * Results:
 *      TRUE if supported by the Host OS/filesystem.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------------
 */

Bool
FileIO_SupportsPrealloc(const char *pathName,  // IN:
                        Bool fsCheck)          // IN:
{
   Bool ret = TRUE;

   if (!HostSupportsPrealloc()) {
      return FALSE;
   }

   if (!fsCheck) {
      return ret;
   }

#if (defined( __linux__) && !defined(VMX86_SERVER))
   {
      struct statfs statBuf;
      Unicode fullPath;

      ret = FALSE;
      if (!pathName) {
         return ret;
      }

      fullPath = File_FullPath(pathName);
      if (!fullPath) {
         return ret;
      }

      if (Posix_Statfs(fullPath, &statBuf) == 0 &&
         statBuf.f_type == EXT4_SUPER_MAGIC) {
         ret = TRUE;
      }
      Unicode_Free(fullPath);
   }
#endif

   return ret;
}

