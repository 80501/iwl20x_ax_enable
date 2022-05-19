#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/livepatch.h>
#include <linux/uuid.h>

int new_iwl_acpi_get_dsm_u32(struct device *dev, int rev, int func,
			 const guid_t *guid, u32 *value);

const guid_t iwl1_guid = GUID_INIT(0xF21202BF, 0x8F78, 0x4DC6,
		  0xA5, 0xB3, 0x1F, 0x73,
		  0x8E, 0x28, 0x5A, 0xDE);
		  
static struct klp_func funcs[] = {
	{
		.old_name = "iwl_acpi_get_dsm_u32",
		.new_func = new_iwl_acpi_get_dsm_u32,
	}, { }
};

static struct klp_object objs[] = {
	{
		.name = "iwlwifi",
		.funcs = funcs,
	}, { }
};

static struct klp_patch patch = {
	.mod = THIS_MODULE,
	.objs = objs,
};

int new_iwl_acpi_get_dsm_u32(struct device *dev, int rev, int func,
			 const guid_t *guid, u32 *value)
{
	if ((func == 0x06)&&(guid_equal(guid, &iwl1_guid)))
	{	
		*value = 0x03;		
		return 0;
	}
	
	return ((int (*)(struct device *, int, int, const guid_t *, 
		u32 *))((funcs->old_func)+MCOUNT_INSN_SIZE))(dev, 
		rev, func, guid, value);
}

static int livepatch_init(void)
{
	return klp_enable_patch(&patch);
}

static void livepatch_exit(void)
{
}

module_init(livepatch_init);
module_exit(livepatch_exit);
MODULE_SOFTDEP("post: iwlwifi");
MODULE_LICENSE("GPL");
MODULE_INFO(livepatch, "Y");


