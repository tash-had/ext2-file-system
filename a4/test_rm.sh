# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#    RM
#       TESTS
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
echo ""
echo ""
echo ""
echo "RM TESTS"
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
