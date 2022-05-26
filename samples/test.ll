; ModuleID = 'unnamed_module'
source_filename = "unnamed_module"

define i64 @main() {
entry:
  %call_tmp = call i64 @add(i64 3, i64 4)
  %add-tmp = add i64 4, %call_tmp
  %call_tmp1 = call i64 @add(i64 1, i64 %add-tmp)
  ret i64 %call_tmp1
}

define i64 @add(i64 %0, i64 %1) {
entry:
  %add-tmp = add i64 %0, %1
  ret i64 %add-tmp
}
