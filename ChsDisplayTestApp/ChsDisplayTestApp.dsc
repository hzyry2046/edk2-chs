## @file
#  Chinese Display Test Application DSC
#
#  This DSC builds the Chinese Display Test Application.
#
#  Copyright (c) 2026, hzyry2046. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[Defines]
  PLATFORM_NAME           = ChsDisplayTestApp
  PLATFORM_GUID           = 8C9CE634-882A-416C-A9C8-77C84FFED81C
  PLATFORM_VERSION        = 0.1
  DSC_SPECIFICATION       = 0x00010005
  OUTPUT_DIRECTORY        = Build/ChsDisplayTestApp
  SUPPORTED_ARCHITECTURES = IA32|X64
  BUILD_TARGETS           = DEBUG|RELEASE
  SKUID_IDENTIFIER        = DEFAULT

[Components]
  ChsDisplayTestApp/ChsDisplayTestApp.inf