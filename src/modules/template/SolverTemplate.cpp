// Copyright (C) 2002 [fill in name]
// Licensed under the GNU GPL Version 2.

#include "ProblemTemplate.h"
#include "EquationTemplate.h"

//-----------------------------------------------------------------------------
const char *ProblemTemplate::Description()
{
	return "My new equation";
}
//-----------------------------------------------------------------------------
void ProblemTemplate::solve()
{
	display->Message(0,"Solving...");	
}
//-----------------------------------------------------------------------------
