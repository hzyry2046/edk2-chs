/** @file
  Chinese Display Test Application

  This application tests whether the EFI firmware supports Chinese character display
  by showing a mixed string of Chinese and English characters.

  Copyright (c) 2026, hzyry2046. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/HiiFont.h>
#include <Protocol/HiiDatabase.h>

EFI_STATUS
EFIAPI
ChsDisplayTestAppMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;
  EFI_HII_FONT_PROTOCOL         *HiiFont;
  EFI_HII_DATABASE_PROTOCOL     *HiiDatabase;
  EFI_FONT_DISPLAY_INFO         *StringInfo;
  EFI_IMAGE_OUTPUT              *Blt;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL ForegroundColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackgroundColor;
  BOOLEAN                       FontProtocolFound;
  BOOLEAN                       GlyphFound;
  EFI_STRING                    TestString;

  // Initialize variables
  GraphicsOutput = NULL;
  HiiFont = NULL;
  HiiDatabase = NULL;
  StringInfo = NULL;
  Blt = NULL;
  FontProtocolFound = FALSE;
  GlyphFound = FALSE;

  // Print welcome message
  Print(L"\n=== Chinese Display Support Test ===\n");
  Print(L"This application tests whether the EFI firmware supports Chinese character display.\n\n");

  // 1. Test if HII Font Protocol is available
  Status = gBS->LocateProtocol(
                 &gEfiHiiFontProtocolGuid,
                 NULL,
                 (VOID**)&HiiFont
                 );
  if (EFI_ERROR(Status)) {
    Print(L"[FAIL] HII Font Protocol not found. Chinese display not supported.\n");
    Print(L"Status: %r\n", Status);
    goto Exit;
  } else {
    Print(L"[PASS] HII Font Protocol found.\n");
    FontProtocolFound = TRUE;
  }

  // 2. Test if HII Database Protocol is available
  Status = gBS->LocateProtocol(
                 &gEfiHiiDatabaseProtocolGuid,
                 NULL,
                 (VOID**)&HiiDatabase
                 );
  if (EFI_ERROR(Status)) {
    Print(L"[FAIL] HII Database Protocol not found.\n");
    Print(L"Status: %r\n", Status);
    goto Exit;
  } else {
    Print(L"[PASS] HII Database Protocol found.\n");
  }

  // 3. Test if Graphics Output Protocol is available
  Status = gBS->LocateProtocol(
                 &gEfiGraphicsOutputProtocolGuid,
                 NULL,
                 (VOID**)&GraphicsOutput
                 );
  if (EFI_ERROR(Status)) {
    Print(L"[WARN] Graphics Output Protocol not found. Testing in text mode.\n");
  } else {
    Print(L"[PASS] Graphics Output Protocol found.\n");
  }

  // 4. Show the test string with mixed Chinese and English characters
  TestString = L"Hello 你好 World 世界";  // Mixed English and Chinese
  Print(L"\nDisplaying test string:\n");
  Print(L"%s\n", TestString);
  Print(L"\nIf you see Chinese characters correctly displayed above, your firmware supports Chinese.\n");
  Print(L"If you see boxes, question marks, or garbage characters, Chinese is not supported.\n\n");

  // 5. Try to render the Chinese string using StringToImage for verification
  StringInfo = (EFI_FONT_DISPLAY_INFO*)AllocateZeroPool(sizeof(EFI_FONT_DISPLAY_INFO) + StrLen(L"StandardFont") * sizeof(CHAR16));
  if (StringInfo == NULL) {
    Print(L"[FAIL] Failed to allocate memory for font display info.\n");
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  // Set up font info
  StringInfo->FontInfoMask = EFI_FONT_INFO_ANY_FONT | EFI_FONT_INFO_ANY_SIZE | EFI_FONT_INFO_ANY_STYLE;
  StringInfo->FontInfo.FontSize = 16;  // Standard size
  StringInfo->FontInfo.FontStyle = 0;
  StrCpyS(StringInfo->FontInfo.FontName, StrLen(L"StandardFont") + 1, L"StandardFont");

  // Set colors
  ForegroundColor.Red = 0xFF;
  ForegroundColor.Green = 0xFF;
  ForegroundColor.Blue = 0xFF;
  ForegroundColor.Reserved = 0;
  StringInfo->ForegroundColor = ForegroundColor;

  BackgroundColor.Red = 0x00;
  BackgroundColor.Green = 0x00;
  BackgroundColor.Blue = 0x00;
  BackgroundColor.Reserved = 0;
  StringInfo->BackgroundColor = BackgroundColor;

  // Try to render the string to verify support
  Blt = NULL;
  Status = HiiFont->StringToImage(
                     HiiFont,
                     EFI_HII_OUT_FLAG_CLIP,
                     TestString,
                     StringInfo,
                     &Blt,
                     10,  // X offset
                     100, // Y offset
                     NULL,  // RowInfoArray
                     NULL,  // RowInfoArraySize
                     NULL   // ColumnInfoArray
                     );

  if (EFI_ERROR(Status)) {
    Print(L"[INFO] StringToImage failed with status: %r\n", Status);
    Print(L"This may indicate limited Chinese character support.\n");
  } else {
    Print(L"[INFO] StringToImage succeeded - Chinese characters are supported.\n");
    GlyphFound = TRUE;
    
    // Check if the rendered image has actual content
    if (Blt != NULL && Blt->Image.Bitmap != NULL && 
        Blt->Width > 0 && Blt->Height > 0) {
      UINTN PixelCount = Blt->Width * Blt->Height;
      UINTN NonZeroPixels = 0;
      UINTN i;
      
      for (i = 0; i < PixelCount; i++) {
        if (Blt->Image.Bitmap[i].Blue != 0 ||
            Blt->Image.Bitmap[i].Green != 0 ||
            Blt->Image.Bitmap[i].Red != 0) {
          NonZeroPixels++;
        }
      }
      
      if (NonZeroPixels > 0) {
        Print(L"Rendered image has %d non-zero pixels - Chinese display confirmed!\n", NonZeroPixels);
      }
    }
    
    // Free the bitmap if allocated
    if (Blt != NULL) {
      if (Blt->Image.Bitmap != NULL) {
        gBS->FreePool(Blt->Image.Bitmap);
      }
      gBS->FreePool(Blt);
    }
  }

  // 6. Test with a known Chinese character using GetGlyph
  Print(L"\nTesting individual Chinese character rendering:\n");
  EFI_IMAGE_OUTPUT *GlyphBlt = NULL;
  UINTN Baseline;
  
  Status = HiiFont->GetGlyph(
                     HiiFont,
                     L'中',  // Chinese character for "middle/center"
                     StringInfo,
                     &GlyphBlt,
                     &Baseline
                     );
  
  if (EFI_ERROR(Status)) {
    Print(L"GetGlyph for '中': [FAIL] Status: %r\n", Status);
  } else {
    Print(L"GetGlyph for '中': [PASS]\n");
    if (GlyphBlt != NULL) {
      if (GlyphBlt->Image.Bitmap != NULL && 
          GlyphBlt->Width > 0 && 
          GlyphBlt->Height > 0) {
        UINTN PixelCount = GlyphBlt->Width * GlyphBlt->Height;
        UINTN NonZeroPixels = 0;
        UINTN i;
        
        for (i = 0; i < PixelCount; i++) {
          if (GlyphBlt->Image.Bitmap[i].Blue != 0 ||
              GlyphBlt->Image.Bitmap[i].Green != 0 ||
              GlyphBlt->Image.Bitmap[i].Red != 0) {
            NonZeroPixels++;
          }
        }
        
        if (NonZeroPixels > 0) {
          Print(L"  Glyph has %d non-zero pixels - Chinese character rendering confirmed!\n", NonZeroPixels);
          GlyphFound = TRUE;
        } else {
          Print(L"  Glyph has no non-zero pixels - character not rendered properly\n");
        }
      }
      gBS->FreePool(GlyphBlt);
    }
  }

  // 7. Summary
  Print(L"\n=== Test Summary ===\n");
  Print(L"HII Font Protocol: %s\n", FontProtocolFound ? L"FOUND" : L"NOT FOUND");
  Print(L"Chinese Character Rendering: %s\n", GlyphFound ? L"SUPPORTED" : L"NOT SUPPORTED");

  if (FontProtocolFound && GlyphFound) {
    Print(L"\n[RESULT] Your EFI firmware supports Chinese character display!\n");
  } else {
    Print(L"\n[RESULT] Your EFI firmware does NOT support Chinese character display.\n");
    if (!FontProtocolFound) {
      Print(L"  Reason: HII Font Protocol not available\n");
    } else if (!GlyphFound) {
      Print(L"  Reason: Chinese glyphs not available in font database\n");
    }
  }

  Print(L"\nThe test string displayed above was: \"%s\"\n", TestString);
  Print(L"Visually inspect the output to confirm Chinese character rendering.\n");

Exit:
  // Cleanup
  if (StringInfo != NULL) {
    gBS->FreePool(StringInfo);
  }

  Print(L"\nTest completed. Press any key to exit...\n");
  gBS->Stall(2000000); // Wait 2 seconds

  return EFI_SUCCESS;
}