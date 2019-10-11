

rm -rf self-tester/runs/*
rm -rf self-tester/results/*

#--- First, copy out the images ---

# Copy
cp images/emptydisk.img self-tester/runs/case1-cp.img
cp images/emptydisk.img self-tester/runs/case2-cp-large.img
cp self-tester/images/removed.img self-tester/runs/case3-cp-rm-dir.img

# Mkdir
cp images/emptydisk.img self-tester/runs/case4-mkdir.img
cp images/onedirectory.img self-tester/runs/case5-mkdir-2.img

# Link
cp images/twolevel.img self-tester/runs/case6-ln-hard.img
cp images/twolevel.img self-tester/runs/case7-ln-soft.img

# Remove
cp self-tester/images/manyfiles.img self-tester/runs/case8-rm.img
cp self-tester/images/manyfiles.img self-tester/runs/case9-rm-2.img
cp self-tester/images/manyfiles.img self-tester/runs/case10-rm-3.img
cp images/largefile.img self-tester/runs/case11-rm-large.img

# Restore
cp self-tester/images/removed.img self-tester/runs/case12-rs.img
cp self-tester/images/removed.img self-tester/runs/case13-rs-2.img
cp self-tester/images/removed-largefile.img self-tester/runs/case14-rs-large.img

# Checker
cp images/twolevel-corrupt.img self-tester/runs/case15-checker.img

#--- Now, do the test cases ---

# Copy
echo "Copy Test 1"
./ext2_cp self-tester/runs/case1-cp.img self-tester/files/oneblock.txt /file.txt
echo "Copy Test 2"
./ext2_cp self-tester/runs/case2-cp-large.img self-tester/files/largefile.txt /big.txt
echo "Copy Test 3"
./ext2_cp self-tester/runs/case3-cp-rm-dir.img self-tester/files/oneblock.txt /level1/file

# Mkdir
echo "Mkdir Test 4"
./ext2_mkdir self-tester/runs/case4-mkdir.img /level1/
echo "Mkdir Test 5"
./ext2_mkdir self-tester/runs/case5-mkdir-2.img /level1/level2

# Link
echo "Link Test 6"
./ext2_ln self-tester/runs/case6-ln-hard.img /level1/level2/bfile /bfilelink
echo "Link test 7"
./ext2_ln self-tester/runs/case7-ln-soft.img -s /level1/level2/bfile /bfilesoftlink

# Remove
echo "Remove Test 8"
./ext2_rm self-tester/runs/case8-rm.img /c.txt
echo "Remove Test 9"
./ext2_rm self-tester/runs/case9-rm-2.img /level1/d.txt
echo "Remove Test 10"
./ext2_rm self-tester/runs/case10-rm-3.img /level1/e.txt
echo "Remove Test 11"
./ext2_rm self-tester/runs/case11-rm-large.img /largefile.txt

# Restore
echo "Restore Test 12"
./ext2_restore self-tester/runs/case12-rs.img /c.txt
echo "Restore Test 13"
./ext2_restore self-tester/runs/case13-rs-2.img /level1/e.txt
echo "Restore Test 14"
./ext2_restore self-tester/runs/case14-rs-large.img /largefile.txt

# Checker
echo "Checker Test 15"
./ext2_checker self-tester/runs/case15-checker.img

# --- Now do the dumps ---
the_files="$(ls self-tester/runs)"
for the_file in $the_files
do
	g=$(basename $the_file)
	./ext2_dump self-tester/runs/$g > self-tester/results/$g.txt
done


