/* 
  input: 1,2,3
  output: 0,1,2
*/
function one_less_list_string(list_string){
  var list = list_string.split(",");
  var result = [];
  for(var i=0; i<list.length; i++){
    result.push(list[i]-1);
  }
  return result.join(",");
}

const a =one_less_list_string("1,28,6,12,9,5,26,29,3,2,20,10,4,15,18,17,14,22,11,19,25,7,23,27,8,24,16,13,21");
console.log(a);