package contract

type OverCostActionArg struct {
	ActionType             int
	TimeTick               int64
	WorkerId               int64
	OverCostCountInOneTerm int
	GenCountInOneTerm      int
	TermIndex              int
}

func (ocaa OverCostActionArg) OverCostActionArg(workerId int64, timeTick int64, actionType int, overCostCountInOneTerm int, genCountWhenOverCost int, index int) {
	ocaa.ActionType = actionType
	ocaa.TimeTick = timeTick
	ocaa.WorkerId = workerId
	ocaa.OverCostCountInOneTerm = overCostCountInOneTerm
	ocaa.GenCountInOneTerm = genCountWhenOverCost
	ocaa.TermIndex = index
}
