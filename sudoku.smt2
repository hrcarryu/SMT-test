; Declare the constants for the grid size
(define-grid-size 9)

; Declare the constants for the values of the grid cells
(define-values 1 2 3 4 5 6 7 8 9)

; Declare the grid as a 2D array of integer variables
(declare-const-array grid Int Int)

; Specify the constraints for each cell
(assert (forall ((i Int) (j Int))
          (let ((cell (select grid i j)))
            (or (and (<= cell 9) (> cell 0))   ; The cell is a value from 1 to 9
                (= cell 0))                     ; The cell is empty
            )))

; Specify the row constraint for each value
(assert (forall ((i Int) (v Int))
          (<= (count ((j Int)) (= (select grid i j) v)) 1)))

; Specify the column constraint for each value
(assert (forall ((j Int) (v Int))
          (<= (count ((i Int)) (= (select grid i j) v)) 1)))

; Specify the small square constraint for each value
(assert (forall ((k Int) (v Int))
          (let ((i-offset (mod k 3))
                (j-offset (div k 3)))
            (<= (count ((i Int) (j Int))
                  (and (= (mod i 3) i-offset)
                       (= (mod j 3) j-offset)
                       (= (select grid i j) v)))
                1))))

; Specify the constraints for the initial values of the grid
(assert (= (select grid 1 1) 5))
(assert (= (select grid 1 2) 3))
(assert (= (select grid 1 5) 7))
(assert (= (select grid 2 1) 6))
(assert (= (select grid 2 4) 1))
(assert (= (select grid 2 5) 9))
(assert (= (select grid 2 6) 5))
(assert (= (select grid 3 2) 9))
(assert (= (select grid 3 3) 8))
(assert (= (select grid 3 8) 6))
(assert (= (select grid 4 1) 8))
(assert (= (select grid 4 5) 6))
(assert (= (select grid 4 9) 3))
(assert (= (select grid 5 1) 4))
(assert (= (select grid 5 4) 8))
(assert (= (select grid 5 6) 3))
(assert (= (select grid 5 9) 1))
(assert (= (select grid 6 1) 7))
(assert (= (select grid 6 5) 2))
(assert (= (select grid 6 9) 6))
(assert (= (select grid 7 2) 6))
(assert (= (select grid 7 7) 8))
(assert (= (select grid 7 8) 2))
(assert (= (select grid 8 4) 4))
(assert (= (select grid 8 5) 1))
(assert (= (select grid 8 6) 9))
(assert (= (select grid 8 9) 5))
(assert (= (select grid 9 7) 7))
(assert (= (select grid 9 8) 9))

; Specify the goal of the problem: find a solution for the grid
(check-sat)
(get-model)
