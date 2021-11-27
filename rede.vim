" Vim syntax file
" Language: rede2 input files
" Author: Pedro Callil
" Written: 26 November 2021


if exists("b:current_syntax")
	finish
endif

" Keywords
syn match genInputFloat '[-+]\d\+\.\d*'
syn match genInputFloat '\d\+\.\d*'
syn match genInputFloat '[+-]\d\+'
syn match genInputFloat '\d\+'

syn keyword genInput rugosity maxiter dampening tolerance viscosity density n
			\ k T0 mu N0 lambda omega T MM Z K Cp/Cv cp/cv
			\ diameter rugosity nextgroup=genInputFloat skipwhite

syn keyword strInput type
syn keyword strInputStr newtonian power law rough pipe power law smooth pipe
			\ ideal gas bingham plastic structural model
syn keyword pipeKeywords contained lenght start end singularities diameter
			\ rugosity external flow pressure
			\ nextgroup=genInputFloat skipwhite

syn match pipeNumber '^\d\+' contained
" syn keyword listInput pipes nodes specifications
syn region pipeBlock start="pipes" end="\n\n"
			\ contains=pipeNumber,pipeKeywords
syn region nodeBlock start="nodes" end="\n\n"
			\ contains=pipeNumber,pipeKeywords
syn region specificationBlock start="specifications" end="\n\n"
			\ contains=pipeNumber,pipeKeywords

let b:current_syntax = "rede"
hi def link genInput Statement
hi def link pipeKeywords Statement
hi def link strInput Statement
hi def link strInputStr String
hi def link pipeNumber PreProc

