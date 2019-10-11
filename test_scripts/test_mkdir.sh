# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#    MKDIR
#       TESTS
# # # # # # # # # # # ## # # # # # # # # # # # # # # # # # # # #
echo ""
echo ""
echo ""
echo "LN TESTS"
sh reloaddisk.sh

./ext2_mkdir images/emptydisk.img /new
read -p "EXT2_MKDIR: CREATED FOLDER new ON emptydisk.img. READ IMAGE? Y/N " answer
case ${answer:0:1} in
    y|Y )
        ./readimage images/emptydisk.img
    ;;
    * )
        echo "====="
    ;;
esac
echo "====="


./ext2_mkdir images/emptydisk.img /new/bit
read -p "EXT2_MKDIR: CREATED FOLDER new/bit ON emptydisk.img. READ IMAGE? Y/N " answer
case ${answer:0:1} in
    y|Y )
        ./readimage images/emptydisk.img
    ;;
    * )
        echo "====="
    ;;
esac
echo "====="

./ext2_mkdir images/emptydisk.img ///new////bit//////trick////
read -p "EXT2_MKDIR: CREATED FOLDER ///new////bit//////trick//// ON emptydisk.img. READ IMAGE? Y/N " answer
case ${answer:0:1} in
    y|Y )
        ./readimage images/emptydisk.img
    ;;
    * )
        echo "====="
    ;;
esac
echo "====="

./ext2_mkdir images/emptydisk.img /new/bit///////
read -p "EXT2_MKDIR: CREATED EXISTING FOLDER new/bit/////// ON emptydisk.img. SEE EXIT CODE? Y/N " answer
case ${answer:0:1} in
    y|Y )
        echo $?
    ;;
    * )
        echo "====="
    ;;
esac
echo "====="

./ext2_mkdir images/emptydisk.img /invalid/bit/
read -p "EXT2_MKDIR: CREATED FOLDER IN INVALID PATH /invalid/bit ON emptydisk.img. SEE EXIT CODE? Y/N " answer
case ${answer:0:1} in
    y|Y )
        echo $?
    ;;
    * )
        echo "====="
    ;;
esac
echo "====="
