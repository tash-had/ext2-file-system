# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#    RESTORE
#       TESTS
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
echo ""
echo ""
echo ""
echo "RESTORE TESTS"
sh reloaddisk.sh


./ext2_cp images/emptydisk.img readimage /
read -p "EXT2_CP: COPIED readimage TO /. READ IMAGE? Y/N " answer
case ${answer:0:1} in
    y|Y )
        ./readimage images/emptydisk.img
    ;;
    * )
        echo "====="
    ;;
esac
echo "====="

./ext2_rm images/emptydisk.img /readimage
read -p "EXT2_RM: COPIED readimage TO /. READ IMAGE? Y/N " answer
case ${answer:0:1} in
    y|Y )
        ./readimage images/emptydisk.img
    ;;
    * )
        echo "====="
    ;;
esac
echo "====="

read -p "RUN ./ext2_restore images/emptydisk.img /readimage ? Y/N " answer
case ${answer:0:1} in
    y|Y )
        ./ext2_restore images/emptydisk.img /readimage
    ;;
    * )
        ./readimage images/emptydisk.img
    ;;
esac
echo "====="
read -p "EXT2_RESTORE: RESTORED readimage TO /. READ IMAGE? Y/N " answer
case ${answer:0:1} in
    y|Y )
        ./readimage images/emptydisk.img
    ;;
    * )
        echo "====="
    ;;
esac
echo "====="

