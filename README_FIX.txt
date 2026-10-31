PHASER V1 PRO - PATCHED BUILD

This package includes defensive validation when restoring host state:
- ignores empty, malformed, or mismatched state chunks
- only replaces APVTS state when the restored ValueTree is valid
- safely handles state serialization if XML creation fails
- stops the editor timer and detaches the editor LookAndFeel on teardown

BUILD ON GITHUB:
1. Upload the contents of this folder to the ROOT of your GitHub repository.
2. Keep CMakeLists.txt at repository root.
3. Commit and push to main (or master).
4. Open Actions > PHASER V1 PRO - Windows Build.
5. Download PHASER-V1-PRO-Windows from Artifacts.

Install the complete PHASER.vst3 bundle folder into:
C:\Program Files\Common Files\VST3

This is a defensive source patch, not a guarantee that the host crash is fixed;
confirm by loading the newly built plugin in FL Studio.
