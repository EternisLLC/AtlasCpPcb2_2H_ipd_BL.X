#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/AtlasCpPcb2_2H_ipd_BL.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/AtlasCpPcb2_2H_ipd_BL.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=DriverMFRC522cp.c crc8.c driverAt45CP.c funcia_spi.c functionCP_220419.c functionRS485_K_220118.c functionUART24.c globalVarAtlasCP.c init256GA106AtlasCP.c interruptTimer24.c terminalCP.c atlasCPmain_211227.c ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.S

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/DriverMFRC522cp.o ${OBJECTDIR}/crc8.o ${OBJECTDIR}/driverAt45CP.o ${OBJECTDIR}/funcia_spi.o ${OBJECTDIR}/functionCP_220419.o ${OBJECTDIR}/functionRS485_K_220118.o ${OBJECTDIR}/functionUART24.o ${OBJECTDIR}/globalVarAtlasCP.o ${OBJECTDIR}/init256GA106AtlasCP.o ${OBJECTDIR}/interruptTimer24.o ${OBJECTDIR}/terminalCP.o ${OBJECTDIR}/atlasCPmain_211227.o ${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o
POSSIBLE_DEPFILES=${OBJECTDIR}/DriverMFRC522cp.o.d ${OBJECTDIR}/crc8.o.d ${OBJECTDIR}/driverAt45CP.o.d ${OBJECTDIR}/funcia_spi.o.d ${OBJECTDIR}/functionCP_220419.o.d ${OBJECTDIR}/functionRS485_K_220118.o.d ${OBJECTDIR}/functionUART24.o.d ${OBJECTDIR}/globalVarAtlasCP.o.d ${OBJECTDIR}/init256GA106AtlasCP.o.d ${OBJECTDIR}/interruptTimer24.o.d ${OBJECTDIR}/terminalCP.o.d ${OBJECTDIR}/atlasCPmain_211227.o.d ${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/DriverMFRC522cp.o ${OBJECTDIR}/crc8.o ${OBJECTDIR}/driverAt45CP.o ${OBJECTDIR}/funcia_spi.o ${OBJECTDIR}/functionCP_220419.o ${OBJECTDIR}/functionRS485_K_220118.o ${OBJECTDIR}/functionUART24.o ${OBJECTDIR}/globalVarAtlasCP.o ${OBJECTDIR}/init256GA106AtlasCP.o ${OBJECTDIR}/interruptTimer24.o ${OBJECTDIR}/terminalCP.o ${OBJECTDIR}/atlasCPmain_211227.o ${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o

# Source Files
SOURCEFILES=DriverMFRC522cp.c crc8.c driverAt45CP.c funcia_spi.c functionCP_220419.c functionRS485_K_220118.c functionUART24.c globalVarAtlasCP.c init256GA106AtlasCP.c interruptTimer24.c terminalCP.c atlasCPmain_211227.c ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.S



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/AtlasCpPcb2_2H_ipd_BL.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ256GA106
MP_LINKER_FILE_OPTION=,--script="ezbl_integration\AtlasCP_pcb2_BOOT_220119.X.merge.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/DriverMFRC522cp.o: DriverMFRC522cp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DriverMFRC522cp.o.d 
	@${RM} ${OBJECTDIR}/DriverMFRC522cp.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DriverMFRC522cp.c  -o ${OBJECTDIR}/DriverMFRC522cp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/DriverMFRC522cp.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/DriverMFRC522cp.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/crc8.o: crc8.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/crc8.o.d 
	@${RM} ${OBJECTDIR}/crc8.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  crc8.c  -o ${OBJECTDIR}/crc8.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/crc8.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/crc8.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/driverAt45CP.o: driverAt45CP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/driverAt45CP.o.d 
	@${RM} ${OBJECTDIR}/driverAt45CP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  driverAt45CP.c  -o ${OBJECTDIR}/driverAt45CP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/driverAt45CP.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/driverAt45CP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/funcia_spi.o: funcia_spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/funcia_spi.o.d 
	@${RM} ${OBJECTDIR}/funcia_spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  funcia_spi.c  -o ${OBJECTDIR}/funcia_spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/funcia_spi.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/funcia_spi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/functionCP_220419.o: functionCP_220419.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/functionCP_220419.o.d 
	@${RM} ${OBJECTDIR}/functionCP_220419.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  functionCP_220419.c  -o ${OBJECTDIR}/functionCP_220419.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/functionCP_220419.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/functionCP_220419.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/functionRS485_K_220118.o: functionRS485_K_220118.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/functionRS485_K_220118.o.d 
	@${RM} ${OBJECTDIR}/functionRS485_K_220118.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  functionRS485_K_220118.c  -o ${OBJECTDIR}/functionRS485_K_220118.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/functionRS485_K_220118.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/functionRS485_K_220118.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/functionUART24.o: functionUART24.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/functionUART24.o.d 
	@${RM} ${OBJECTDIR}/functionUART24.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  functionUART24.c  -o ${OBJECTDIR}/functionUART24.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/functionUART24.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/functionUART24.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/globalVarAtlasCP.o: globalVarAtlasCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/globalVarAtlasCP.o.d 
	@${RM} ${OBJECTDIR}/globalVarAtlasCP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  globalVarAtlasCP.c  -o ${OBJECTDIR}/globalVarAtlasCP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/globalVarAtlasCP.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/globalVarAtlasCP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/init256GA106AtlasCP.o: init256GA106AtlasCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/init256GA106AtlasCP.o.d 
	@${RM} ${OBJECTDIR}/init256GA106AtlasCP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  init256GA106AtlasCP.c  -o ${OBJECTDIR}/init256GA106AtlasCP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/init256GA106AtlasCP.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/init256GA106AtlasCP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/interruptTimer24.o: interruptTimer24.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interruptTimer24.o.d 
	@${RM} ${OBJECTDIR}/interruptTimer24.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interruptTimer24.c  -o ${OBJECTDIR}/interruptTimer24.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interruptTimer24.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/interruptTimer24.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/terminalCP.o: terminalCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/terminalCP.o.d 
	@${RM} ${OBJECTDIR}/terminalCP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  terminalCP.c  -o ${OBJECTDIR}/terminalCP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/terminalCP.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/terminalCP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/atlasCPmain_211227.o: atlasCPmain_211227.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/atlasCPmain_211227.o.d 
	@${RM} ${OBJECTDIR}/atlasCPmain_211227.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  atlasCPmain_211227.c  -o ${OBJECTDIR}/atlasCPmain_211227.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/atlasCPmain_211227.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1    -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/atlasCPmain_211227.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/DriverMFRC522cp.o: DriverMFRC522cp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/DriverMFRC522cp.o.d 
	@${RM} ${OBJECTDIR}/DriverMFRC522cp.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  DriverMFRC522cp.c  -o ${OBJECTDIR}/DriverMFRC522cp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/DriverMFRC522cp.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/DriverMFRC522cp.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/crc8.o: crc8.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/crc8.o.d 
	@${RM} ${OBJECTDIR}/crc8.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  crc8.c  -o ${OBJECTDIR}/crc8.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/crc8.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/crc8.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/driverAt45CP.o: driverAt45CP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/driverAt45CP.o.d 
	@${RM} ${OBJECTDIR}/driverAt45CP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  driverAt45CP.c  -o ${OBJECTDIR}/driverAt45CP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/driverAt45CP.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/driverAt45CP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/funcia_spi.o: funcia_spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/funcia_spi.o.d 
	@${RM} ${OBJECTDIR}/funcia_spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  funcia_spi.c  -o ${OBJECTDIR}/funcia_spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/funcia_spi.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/funcia_spi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/functionCP_220419.o: functionCP_220419.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/functionCP_220419.o.d 
	@${RM} ${OBJECTDIR}/functionCP_220419.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  functionCP_220419.c  -o ${OBJECTDIR}/functionCP_220419.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/functionCP_220419.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/functionCP_220419.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/functionRS485_K_220118.o: functionRS485_K_220118.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/functionRS485_K_220118.o.d 
	@${RM} ${OBJECTDIR}/functionRS485_K_220118.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  functionRS485_K_220118.c  -o ${OBJECTDIR}/functionRS485_K_220118.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/functionRS485_K_220118.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/functionRS485_K_220118.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/functionUART24.o: functionUART24.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/functionUART24.o.d 
	@${RM} ${OBJECTDIR}/functionUART24.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  functionUART24.c  -o ${OBJECTDIR}/functionUART24.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/functionUART24.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/functionUART24.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/globalVarAtlasCP.o: globalVarAtlasCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/globalVarAtlasCP.o.d 
	@${RM} ${OBJECTDIR}/globalVarAtlasCP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  globalVarAtlasCP.c  -o ${OBJECTDIR}/globalVarAtlasCP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/globalVarAtlasCP.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/globalVarAtlasCP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/init256GA106AtlasCP.o: init256GA106AtlasCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/init256GA106AtlasCP.o.d 
	@${RM} ${OBJECTDIR}/init256GA106AtlasCP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  init256GA106AtlasCP.c  -o ${OBJECTDIR}/init256GA106AtlasCP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/init256GA106AtlasCP.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/init256GA106AtlasCP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/interruptTimer24.o: interruptTimer24.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interruptTimer24.o.d 
	@${RM} ${OBJECTDIR}/interruptTimer24.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interruptTimer24.c  -o ${OBJECTDIR}/interruptTimer24.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interruptTimer24.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/interruptTimer24.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/terminalCP.o: terminalCP.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/terminalCP.o.d 
	@${RM} ${OBJECTDIR}/terminalCP.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  terminalCP.c  -o ${OBJECTDIR}/terminalCP.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/terminalCP.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/terminalCP.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/atlasCPmain_211227.o: atlasCPmain_211227.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/atlasCPmain_211227.o.d 
	@${RM} ${OBJECTDIR}/atlasCPmain_211227.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  atlasCPmain_211227.c  -o ${OBJECTDIR}/atlasCPmain_211227.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/atlasCPmain_211227.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/atlasCPmain_211227.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o: ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/ezbl_integration" 
	@${RM} ${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o.d 
	@${RM} ${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.S  -o ${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o.d"  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -Wa,-MD,"${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o.d" "${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o: ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/ezbl_integration" 
	@${RM} ${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o.d 
	@${RM} ${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.S  -o ${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o.d"  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -Wa,-MD,"${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)  -mdfp=${DFP_DIR}/xc16
	@${FIXDEPS} "${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o.d" "${OBJECTDIR}/ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/AtlasCpPcb2_2H_ipd_BL.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ezbl_integration/ezbl_lib.a  ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/AtlasCpPcb2_2H_ipd_BL.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}    ezbl_integration\ezbl_lib.a  -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x800:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x825 -mreserve=data@0x826:0x84F   -Wl,,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp=${DFP_DIR}/xc16 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/AtlasCpPcb2_2H_ipd_BL.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ezbl_integration/ezbl_lib.a ezbl_integration/AtlasCP_pcb2_BOOT_220119.X.merge.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/AtlasCpPcb2_2H_ipd_BL.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}    ezbl_integration\ezbl_lib.a  -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -Wl,,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp=${DFP_DIR}/xc16 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/AtlasCpPcb2_2H_ipd_BL.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   -mdfp=${DFP_DIR}/xc16 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
