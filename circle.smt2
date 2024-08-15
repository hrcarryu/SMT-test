(set-logic QF_NRA)  ; 设置逻辑为无量化符号的非线性实数算术

(declare-fun x () Real)  ; 声明实数变量 x
(declare-fun y () Real)  ; 声明实数变量 y

; 约束条件：x^2 + y^2 = 1
(assert (= (+ (* x x) (* y y)) 1))

; 优化目标：minimize x + y
(minimize (+ x y))

(check-sat)  ; 检查可满足性
(get-objectives)  ; 获取目标函数的最小值

(exit)  ; 退出