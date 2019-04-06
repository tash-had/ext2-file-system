# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#    LN
#       TESTS
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
echo ""
echo ""
echo ""
echo "LN TESTS"
sh reloaddisk.sh

./ext2_ln images/emptydisk.img /readimage /readimghardlnk
read -p "EXT2_CP: COPIED readimage TO /test. READ IMAGE? Y/N " answer
case ${answer:0:1} in
    y|Y )
        ./readimage images/emptydisk.img
    ;;
    * )
        echo "====="
    ;;
esac
echo "====="