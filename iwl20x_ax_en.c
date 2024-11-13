#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/livepatch.h>
#include <linux/uuid.h>
#include <linux/kprobes.h>

#define AX_UKRAINE_ENABLE	0x03
#define AX_RUSSIA_ENABLE	0x0c
#define AX_ENABLE_BITMAP	AX_UKRAINE_ENABLE|AX_RUSSIA_ENABLE

enum iwl_dsm_funcs {
    DSM_FUNC_QUERY = 0,
    DSM_FUNC_DISABLE_SRD = 1,
    DSM_FUNC_ENABLE_INDONESIA_5G2 = 2,
    DSM_FUNC_ENABLE_6E = 3,
    DSM_FUNC_REGULATORY_CONFIG = 4,
    DSM_FUNC_11AX_ENABLEMENT = 6,
    DSM_FUNC_ENABLE_UNII4_CHAN = 7,
    DSM_FUNC_ACTIVATE_CHANNEL = 8,
    DSM_FUNC_FORCE_DISABLE_CHANNELS = 9,
    DSM_FUNC_ENERGY_DETECTION_THRESHOLD = 10,
    DSM_FUNC_RFI_CONFIG = 11,
    DSM_FUNC_ENABLE_11BE = 12,
    DSM_FUNC_NUM_FUNCS = 13,
};

int new_iwl_acpi_get_dsm_u32(struct device *dev, int rev, int func,
			 const guid_t *guid, u32 *value);
			 
int new_iwl_bios_get_dsm(void *fwrt, enum iwl_dsm_funcs func,
	     u32 *value);

const guid_t iwl1_guid = GUID_INIT(0xF21202BF, 0x8F78, 0x4DC6,
		  0xA5, 0xB3, 0x1F, 0x73,
		  0x8E, 0x28, 0x5A, 0xDE);


static struct klp_func func_acpi_get_dsm_u32[] = {
	{
		.old_name = "iwl_acpi_get_dsm_u32",
		.new_func = new_iwl_acpi_get_dsm_u32,
	}, { }
};

static struct klp_func func_iwl_bios_get_dsm[] = {
	{
		.old_name = "iwl_bios_get_dsm",
		.new_func = new_iwl_bios_get_dsm,
	}, { }
};

static struct klp_object objs[] = {
	{
		.name = "iwlwifi",
		.funcs = NULL,
	}, { }
};

static struct klp_patch patch = {
	.mod = THIS_MODULE,
	.objs = objs,
};

int new_iwl_acpi_get_dsm_u32(struct device *dev, int rev, int func,
			 const guid_t *guid, u32 *value)
{
	__maybe_unused int r_code;

	if ((func == DSM_FUNC_11AX_ENABLEMENT)&&(guid_equal(guid, &iwl1_guid)))
	{	
		*value = AX_ENABLE_BITMAP;		
		return 0;
	}
	
	#ifdef CONFIG_X86_KERNEL_IBT
	func_acpi_get_dsm_u32->nop=true;
	r_code=((int (*)(struct device *, int, int, const guid_t *, 
		u32 *))(func_acpi_get_dsm_u32->old_func))(dev, 
		rev, func, guid, value);
	func_acpi_get_dsm_u32->nop=false;
	return r_code;

	#else	
	return ((int (*)(struct device *, int, int, const guid_t *, 
		u32 *))((func_acpi_get_dsm_u32->old_func)+MCOUNT_INSN_SIZE))(dev, 
		rev, func, guid, value);
	#endif
}

int new_iwl_bios_get_dsm(void *fwrt, enum iwl_dsm_funcs func,
		 u32 *value)
{
	__maybe_unused int r_code;

	if (func == DSM_FUNC_11AX_ENABLEMENT)
	{	
		*value = AX_ENABLE_BITMAP;
		return 0;
	}
		
	#ifdef CONFIG_X86_KERNEL_IBT
	func_iwl_bios_get_dsm->nop=true;
	r_code=((int (*)(void *, enum iwl_dsm_funcs, u32 *))
	     	(func_iwl_bios_get_dsm->old_func))
		(fwrt, func, value);
	func_iwl_bios_get_dsm->nop=false;

	return r_code;
	
	#else
	return ((int (*)(void *, enum iwl_dsm_funcs, u32 *))
		((func_iwl_bios_get_dsm->old_func)+MCOUNT_INSN_SIZE))
	     	(fwrt, func, value);
	#endif
}

static int lookup_fn(const char *fn_name)
{
	struct kprobe test_kp;
	int ret;
	
	memset(&test_kp, 0, sizeof(struct kprobe));
	test_kp.symbol_name = fn_name;
	ret = register_kprobe(&test_kp);
	unregister_kprobe(&test_kp);
	
	return ret;
}

static int livepatch_init(void)
{

	if (lookup_fn(func_iwl_bios_get_dsm->old_name) == 0)
		objs->funcs = func_iwl_bios_get_dsm;		
	if (lookup_fn(func_acpi_get_dsm_u32->old_name) == 0)
		objs->funcs = func_acpi_get_dsm_u32;
	if (objs->funcs == NULL)
		return -ENOENT;

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
