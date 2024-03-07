echo "Number of threads to compile with: "
read thread_count

cd ../../

python3 thirdparty/scons/scons.py target=release_debug tools=yes platform=osx bits=64 -j$thread_count

rm -rf "bin/godot_2_editor.app"
cp -r "misc/dist/osx_tools.app" "bin/godot_2_editor.app"
mkdir -p "bin/godot_2_editor.app/Contents/MacOS"
cp "bin/godot.osx.opt.tools.64" "bin/godot_2_editor.app/Contents/MacOS/Godot"
chmod +x "bin/godot_2_editor.app/Contents/MacOS/Godot"
