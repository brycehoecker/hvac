BEGIN{}
#($3 == 6){printf("%d %d\n",$1,$4) > "board6.txt"}
($3 == 4){printf("%d %d %d %d\n",$3,$4,$1,$5) > "board4.txt"}
#{if(($3==2) || ($3==8)){
#  printf("%d %d %d %d %d\n",$1,$2,$3,$4,850) > "spit.out"
# }
# else
# {
#  printf("%d %d %d %d %d\n",$1,$2,$3,$4,$5) > "spit.out"
# }
#}
#{if(($3==8) && ($5 >=950) || ($5 == 0)){
#  printf("%d %d %d %d %d\n",$1,$2,$3,$4,930) > "spit.out"
# }
# else
# {
#  printf("%d %d %d %d %d\n",$1,$2,$3,$4,$5) > "spit.out"
# }
#}
END{}
