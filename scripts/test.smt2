(set-info :smt-lib-version 2.6)
(set-logic QF_LIA)
(set-info :source |
Generated by: Nicolas Amat, Bernard Berthomieu, Silvano Dal Zilio, Didier Le Botlan
Generated on: 2022-03-07
Generator: SMPT
Application: Reachability query on the instance RobotManipulation-PT-00005 from the Model Checking Contest 2021 using the SMPT model-checker
Target solver: z3
Publications: https://doi.org/10.1007/978-3-030-76983-3_9, http://mcc.lip6.fr/2021/results.php
|)
(set-info :license "https://creativecommons.org/licenses/by/4.0/")
(set-info :category "industrial")
(set-info :status sat)

(declare-const access Int)
(assert (>= access 0))
(declare-const p_rdy Int)
(assert (>= p_rdy 0))
(declare-const p_sc Int)
(assert (>= p_sc 0))
(declare-const move Int)
(assert (>= move 0))
(declare-const p_m Int)
(assert (>= p_m 0))
(declare-const moved Int)
(assert (>= moved 0))
(declare-const p_rel Int)
(assert (>= p_rel 0))
(declare-const off Int)
(assert (>= off 0))
(declare-const p_i1 Int)
(assert (>= p_i1 0))
(declare-const initialize Int)
(assert (>= initialize 0))
(declare-const p_i2 Int)
(assert (>= p_i2 0))
(declare-const initialized Int)
(assert (>= initialized 0))
(declare-const r_active Int)
(assert (>= r_active 0))
(declare-const r_moving Int)
(assert (>= r_moving 0))
(declare-const r_stopped Int)
(assert (>= r_stopped 0))

(declare-const aa1 Int)
(assert (>= aa1 0))
(declare-const aa2 Int)
(assert (>= aa2 0))
(declare-const aa3 Int)
(assert (>= aa3 0))
(declare-const aa4 Int)
(assert (>= aa4 0))
(declare-const aa5 Int)
(assert (>= aa5 0))
(declare-const aa6 Int)
(assert (>= aa6 0))
(declare-const aa7 Int)
(assert (>= aa7 0))
(declare-const aa8 Int)
(assert (>= aa8 0))
(declare-const aa9 Int)
(assert (>= aa9 0))

(assert (= access (+ initialized off p_rdy r_stopped)))
(assert (= p_i2 (+ initialize initialized)))
(assert (= p_m (+ move moved r_moving)))
(assert (= r_active (+ initialized move moved off p_rdy p_rel p_sc)))
(assert (= aa1 (+ r_moving move)))
(assert (= aa2 (+ p_rel moved)))
(assert (= aa3 (+ initialize p_i1)))
(assert (= aa4 (+ p_sc p_rdy)))
(assert (= aa5 (+ aa2 aa1)))
(assert (= aa6 (+ aa5 aa4)))
(assert (= aa7 (+ aa6 initialized)))
(assert (= aa3 (+ off r_stopped 1)))
(assert (= aa8 (+ off aa7)))
(assert (= aa9 (+ aa8 r_stopped)))
(assert (= aa9 10))

(assert (and (not (and (not (>= p_rel 1))(or (and (>= initialize 1)(>= r_stopped 1))(and (>= off 1)(>= r_active 1)))))(and (not (or (>= p_rdy 1)(>= p_rdy 1)))(or (and (>= p_rdy 1)(and (>= initialized 1)(>= p_i2 1)))(or (>= p_i1 1)(>= p_rdy 1))))))

(maximize aa3)
(check-sat)
(exit)
