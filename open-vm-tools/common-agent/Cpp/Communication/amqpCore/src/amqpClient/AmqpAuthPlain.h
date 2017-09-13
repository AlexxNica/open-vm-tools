/*
 *  Created on: Oct 7, 2014
 *      Author: bwilliams
 *
 *  Copyright (c) 2014 VMware, Inc.  All rights reserved.
 *  -- VMware Confidential
 */

#ifndef AMQPCORE_AMQPAUTHPLAIN_H_
#define AMQPCORE_AMQPAUTHPLAIN_H_

namespace Caf { namespace AmqpClient {

#define	SHA256_HASH_LEN	32

class AmqpAuthPlain {
public:
	static AMQPStatus AMQP_AuthPlainCreateClient(
			SmartPtrCAmqpAuthMechanism& pMech,
			const std::string& username,
			const std::string& password);

private:
  CAF_CM_DECLARE_NOCREATE (AmqpAuthPlain);
};

}}

#endif /* AMQPCORE_AMQPAUTHPLAIN_H_ */
