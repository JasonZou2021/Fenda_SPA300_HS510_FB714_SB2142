
bracket_left :=(
bracket_right :=)
mcode_i:=$(OUT_ROOT)/mcode.i
mcode_inc:=$(OUT_ROOT)/mcode.inc
mem_ldp:=$(OUT_ROOT)/mem.ldp
MCODE_MODLIST:=$(filter-out iop iop_rst,$(REALMODULE_LIST))
define gen_mcode_inc =
$(eval prev_m:=CODE) \
$(file >$(mcode_inc), ) \
$(file >>$(mcode_inc),#define CODE_SECTION_ST		$(bracket_left)0$(bracket_right))\
$(file >>$(mcode_inc), ) \
$(foreach m,$(MCODE_MODLIST),$(eval upper_$(m):=$(shell echo $(m)|awk '{print toupper($$0)}')) \
$(if $(filter $(m),base),\
$(file >>$(mcode_inc),#define $(upper_$(m))_CODE_ST		$(bracket_left)CODE_SECTION_ST$(bracket_right)),\
$(file >>$(mcode_inc),#define $(upper_$(m))_CODE_ST		$(bracket_left)$(prev_m)_CODE_ST+$(prev_m)_CODE_SIZE$(bracket_right)))\
$(file >>$(mcode_inc),$$#define $(upper_$(m))_CODE_SIZE		$(bracket_left)$(m)$(bracket_right))\
$(file >>$(mcode_inc), ) \
$(eval prev_m:=$(upper_$(m))))
endef

define gen_mcode_i =
$(eval prev_m:=CODE) \
$(file >$(mcode_i), ) \
$(file >>$(mcode_i),#define CODE_SECTION_ST		$(bracket_left)0$(bracket_right))\
$(file >>$(mcode_inc), ) \
$(foreach m,$(MCODE_MODLIST),$(eval upper_$(m):=$(shell echo $(m)|awk '{print toupper($$0)}')) \
$(if $(filter $(m),base),\
$(file >>$(mcode_i),#define $(upper_$(m))_CODE_ST		$(bracket_left)CODE_SECTION_ST$(bracket_right)),\
$(file >>$(mcode_i),#define $(upper_$(m))_CODE_ST		$(bracket_left)$(prev_m)_CODE_ST+$(prev_m)_CODE_SIZE$(bracket_right)))\
$(file >>$(mcode_i),#define $(upper_$(m))_CODE_SIZE		$(bracket_left)4$(bracket_right))\
$(file >>$(mcode_i), ) \
$(eval prev_m:=$(upper_$(m))))
endef

define gen_ldp_mem =
$(file >$(mem_ldp), ) \
$(foreach m,$(MCODE_MODLIST),$(eval upper_$(m):=$(shell echo $(m)|awk '{print toupper($$0)}')) \
$(file >>$(mem_ldp),    sdram_$(m)	: org = TO080$(bracket_left)$(upper_$(m))_CODE_ST$(bracket_right),	l = K$(bracket_left)$(upper_$(m))_CODE_SIZE$(bracket_right))\
)
endef

.PHONY:gen_mcodei
gen_mcodei:
	$(eval $(call gen_mcode_i))
	$(eval $(call gen_mcode_inc))
	$(eval $(call gen_ldp_mem))
	@$(ECHO) generate ok: $(mcode_i) $(mcode_inc)
