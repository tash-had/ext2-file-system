# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#    LN
#       TESTS
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
echo ""
echo ""
echo ""
echo "LN TESTS"
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

./ext2_ln images/emptydisk.img /readimage /readimghardlnk
read -p "EXT2_LN: LN called readimghrdlnk . READ IMAGE? Y/N " answer
case ${answer:0:1} in
    y|Y )
        ./readimage images/emptydisk.img
    ;;
    * )
        echo "====="
    ;;
esac
echo "====="