#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.arm.elf.M4F{1,0,5.1,5
#
ifeq (,$(MK_NOGENDEPS))
-include package/cfg/main_pem4f.oem4f.dep
package/cfg/main_pem4f.oem4f.dep: ;
endif

package/cfg/main_pem4f.oem4f: | .interfaces
package/cfg/main_pem4f.oem4f: package/cfg/main_pem4f.c package/cfg/main_pem4f.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clem4f $< ...
	$(ti.targets.arm.elf.M4F.rootDir)/bin/armcl -c  -qq -pdsw225 -ms --fp_mode=strict --endian=little -mv7M4 --abi=eabi --float_support=fpv4spd16 -eo.oem4f -ea.sem4f  --embed_inline_assembly  -Dxdc_cfg__xheader__='"xconfig_main/package/cfg/main_pem4f.h"'  -Dxdc_target_name__=M4F -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_5_1_5 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M4F.rootDir)/include/rts -I$(ti.targets.arm.elf.M4F.rootDir)/include  -fs=./package/cfg -fr=./package/cfg -fc $<
	$(MKDEP) -a $@.dep -p package/cfg -s oem4f $< -C   -qq -pdsw225 -ms --fp_mode=strict --endian=little -mv7M4 --abi=eabi --float_support=fpv4spd16 -eo.oem4f -ea.sem4f  --embed_inline_assembly  -Dxdc_cfg__xheader__='"xconfig_main/package/cfg/main_pem4f.h"'  -Dxdc_target_name__=M4F -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_5_1_5 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M4F.rootDir)/include/rts -I$(ti.targets.arm.elf.M4F.rootDir)/include  -fs=./package/cfg -fr=./package/cfg
	-@$(FIXDEP) $@.dep $@.dep
	
package/cfg/main_pem4f.oem4f: export C_DIR=
package/cfg/main_pem4f.oem4f: PATH:=$(ti.targets.arm.elf.M4F.rootDir)/bin/;$(PATH)
package/cfg/main_pem4f.oem4f: Path:=$(ti.targets.arm.elf.M4F.rootDir)/bin/;$(PATH)

package/cfg/main_pem4f.sem4f: | .interfaces
package/cfg/main_pem4f.sem4f: package/cfg/main_pem4f.c package/cfg/main_pem4f.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) clem4f -n $< ...
	$(ti.targets.arm.elf.M4F.rootDir)/bin/armcl -c -n -s --symdebug:none -qq -pdsw225 --endian=little -mv7M4 --abi=eabi --float_support=fpv4spd16 -eo.oem4f -ea.sem4f   -Dxdc_cfg__xheader__='"xconfig_main/package/cfg/main_pem4f.h"'  -Dxdc_target_name__=M4F -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_5_1_5 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M4F.rootDir)/include/rts -I$(ti.targets.arm.elf.M4F.rootDir)/include  -fs=./package/cfg -fr=./package/cfg -fc $<
	$(MKDEP) -a $@.dep -p package/cfg -s oem4f $< -C  -n -s --symdebug:none -qq -pdsw225 --endian=little -mv7M4 --abi=eabi --float_support=fpv4spd16 -eo.oem4f -ea.sem4f   -Dxdc_cfg__xheader__='"xconfig_main/package/cfg/main_pem4f.h"'  -Dxdc_target_name__=M4F -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_5_1_5 -O2  $(XDCINCS) -I$(ti.targets.arm.elf.M4F.rootDir)/include/rts -I$(ti.targets.arm.elf.M4F.rootDir)/include  -fs=./package/cfg -fr=./package/cfg
	-@$(FIXDEP) $@.dep $@.dep
	
package/cfg/main_pem4f.sem4f: export C_DIR=
package/cfg/main_pem4f.sem4f: PATH:=$(ti.targets.arm.elf.M4F.rootDir)/bin/;$(PATH)
package/cfg/main_pem4f.sem4f: Path:=$(ti.targets.arm.elf.M4F.rootDir)/bin/;$(PATH)

clean,em4f ::
	-$(RM) package/cfg/main_pem4f.oem4f
	-$(RM) package/cfg/main_pem4f.sem4f

main.pem4f: package/cfg/main_pem4f.oem4f package/cfg/main_pem4f.mak

clean::
	-$(RM) package/cfg/main_pem4f.mak
