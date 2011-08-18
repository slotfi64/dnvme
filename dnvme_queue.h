#ifndef _DNVME_QUEUE_H_
#define _DNVME_QUEUE_H_

/* Admin SQ tail Door bell offset */
#define NVME_SQ0TBDL	0x1000

/* Admin SQ size Mask bits 0-11 in AQA */
#define	ASQS_MASK	0xFFF

/* Admin Completion Q Mask Bits 16-21 in ADA */
#define ACQS_MASK	0x0FFF0000

/* As Time Out is in lower 32 bits of 64 bit CAP */
#define NVME_TO_MASK	0xFF000000

/*
* Each unit in TO is 500 ms, converting ms to jiffies
*/
#define NVME_MSEC_2_JIFFIES (500 * HZ / 1000)

/*
* Enumerating the differnt NVME Controller Capabilities of the
* PCI Express device as per NVME Spec 1.0a.
*/
enum {
	NVME_CC_ENABLE		= 1 << 0,
	NVME_CC_CSS_NVM		= 0 << 4,
	NVME_CC_MPS_SHIFT	= 7,
	NVME_CC_ARB_RR		= 0 << 11,
	NVME_CC_ARB_WRRU	= 1 << 11,
	NVME_CC_ARB_VS		= 3 << 11,
	NVME_CC_SHN_NONE	= 0 << 13,
	NVME_CC_SHN_NORMAL	= 1 << 13,
	NVME_CC_SHN_ABRUPT	= 2 << 13,
	NVME_CSTS_RDY		= 1 << 0,
	NVME_CSTS_CFS		= 1 << 1,
	NVME_CSTS_SHST_NORMAL	= 0 << 2,
	NVME_CSTS_SHST_OCCUR	= 1 << 2,
	NVME_CSTS_SHST_CMPLT	= 2 << 2,
};

/*
* Command structure with parameters for creating SQ.
*/
struct nvme_create_sq {
	__u16 comand_id;
};

/*
* Nvme Structure for different command sets.
*/
struct nvme_command {
	struct nvme_create_sq create_sq;
};

/*
* Structure definition for the NVME Express Queue. This is the main entry
* point for all the Q's that the NVME device functions with and manipulates.
*/
struct nvme_queue {
	struct	device		*dmadev;
	struct	nvme_dev_entry	*dev;
	spinlock_t		q_lock;
	struct	nvme_command	*nvme_sq_cmds;
	void			*virt_asq_addr;
	void			*virt_acq_addr;
	dma_addr_t		asq_dma_addr;
	dma_addr_t		acq_dma_addr;
	u16			asq_depth;
	u16			acq_depth;
	u16			sq_head;
	u16			sq_tail;
	u8			q_init;
};


/**
* The user selection of IOCTL for creating admin cq eventually calls
* this function if init is successful. This will create infrastructure
* for Admin Completion Q creation
* @param nvme_dev
* @param qsize
* @return whether ACQ creation successful or not.
*/
int create_admn_cq(struct nvme_dev_entry *nvme_dev, u16 qsize);

/**
* The user selection of IOCTL for creating admin sq eventually calls
* this function if init is successful. This will create infrastructure
* for Admin Submission Q creation
* @param nvme_dev
* @param qsize
* @return whether ASQ creation successful or not.
*/
int create_admn_sq(struct nvme_dev_entry *nvme_dev, u16 qsize);

/**
* this function initialized Q parameters. This will create infrastructure
* for Admin Submission Q creation and Admin Completion Q creation
* @param nvme_dev
* @param qsize
* @return whether initailization was success or not.
*/
int nvme_queue_init(struct nvme_dev_entry *nvme_dev, u16 qsize);

/**
* This is the timer handler which will be invoked by the kernel when the timer
* expires in the timer.expires field. This function will set a flag which is
* used by the create admn sq routine to exit.
* @param arg
*/
void jit_timer_fn(unsigned long arg);


#endif
