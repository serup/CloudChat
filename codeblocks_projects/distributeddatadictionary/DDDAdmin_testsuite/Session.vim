let SessionLoad = 1
if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
inoremap <C-Space> 
imap <Nul> <C-Space>
inoremap <expr> <Up> pumvisible() ? "\" : "\<Up>"
inoremap <expr> <S-Tab> pumvisible() ? "\" : "\<S-Tab>"
inoremap <expr> <Down> pumvisible() ? "\" : "\<Down>"
noremap m =
noremap M | _
nnoremap ,d :YcmShowDetailedDiagnostic
map ,rwp <Plug>RestoreWinPosn
map ,swp <Plug>SaveWinPosn
noremap <silent> ,tb :TagbarToggle
noremap <silent> ,jb 
noremap <silent> ,jf 
noremap <silent> ,fa :%g/^{/normal! zf%
nnoremap <silent> ,ww :call WindowSwap#EasyWindowSwap()
nnoremap <silent> ,pw :call WindowSwap#DoWindowSwap()
nnoremap <silent> ,yw :call WindowSwap#MarkWindowSwap()
nnoremap \d :YcmShowDetailedDiagnostic
map \rwp <Plug>RestoreWinPosn
map \swp <Plug>SaveWinPosn
vmap gx <Plug>NetrwBrowseXVis
nmap gx <Plug>NetrwBrowseX
nnoremap <SNR>27_: :=v:count ? v:count : ''
nnoremap <SNR>19_: :=v:count ? v:count : ''
nnoremap <silent> <F10> :call conque_term#send_file()
nnoremap <F8> :call conque_term#set_mappings("toggle")
nnoremap <SNR>18_: :=v:count ? v:count : ''
vnoremap <silent> <Plug>NetrwBrowseXVis :call netrw#BrowseXVis()
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#BrowseX(expand((exists("g:netrw_gx")? g:netrw_gx : '<cfile>')),netrw#CheckIfRemote())
nnoremap <silent> <Plug>(startify-open-buffers) :call startify#open_buffers()
nnoremap <SNR>29_: :=v:count ? v:count : ''
nnoremap <silent> <F11> :call conque_term#exec_file()
nmap <silent> <Plug>RestoreWinPosn :call RestoreWinPosn()
nmap <silent> <Plug>SaveWinPosn :call SaveWinPosn()
noremap <F3> :so test_project.vim 
map <F9> :so build_project.vim
noremap <F2> :AnsiEsc
inoremap <expr> 	 pumvisible() ? "\" : "\	"
let &cpo=s:cpo_save
unlet s:cpo_save
set background=dark
set backspace=indent,eol,start
set completefunc=youcompleteme#Complete
set completeopt=preview,menuone
set cpoptions=ceFsB
set fileencodings=ucs-bom,utf-8,default,latin1
set helplang=en
set highlight=8:Ignore,@:NonText,d:Directory,e:ErrorMsg,i:IncSearch,l:Search,m:MoreMsg,M:ModeMsg,n:LineNr,N:CursorLineNr,r:Question,s:StatusLine,S:StatusLineNC,c:VertSplit,t:Title,v:Visual,V:VisualNOS,w:WarningMsg,W:WildMenu,f:Folded,F:FoldColumn,A:DiffAdd,C:DiffChange,D:DiffDelete,T:DiffText,>:SignColumn,-:Conceal,B:SpellBad,P:SpellCap,R:SpellRare,L:SpellLocal,+:Pmenu,=:PmenuSel,x:PmenuSbar,X:PmenuThumb,*:TabLine,#:TabLineSel,_:TabLineFill,!:CursorColumn,.:CursorLine,o:ColorColumn
set laststatus=2
set mouse=a
set omnifunc=youcompleteme#OmniComplete
set printoptions=paper:letter
set report=10000
set ruler
set runtimepath=~/.vim,~/.vim/bundle/tagbar,~/.vim/bundle/vim-autotag,~/.vim/bundle/vim-windowswap,~/.vim/bundle/Vundle.vim,~/.vim/bundle/vim-fugitive,~/.vim/bundle/L9,~/.vim/bundle/sparkup/vim/,~/.vim/bundle/YouCompleteMe,~/.vim/bundle/vim-startify,/var/lib/vim/addons,/usr/share/vim/vimfiles,/usr/share/vim/vim74,/usr/share/vim/vim74/pack/dist/opt/editexisting,/usr/share/vim/vimfiles/after,/var/lib/vim/addons/after,~/.vim/after,~/.vim/bundle/Vundle.vim,~/.vim/bundle/Vundle.vim/after,~/.vim/bundle/vim-fugitive/after,~/.vim/bundle/L9/after,~/.vim/bundle/sparkup/vim//after,~/.vim/bundle/YouCompleteMe/after,~/.vim/bundle/vim-startify/after,~/.vim/bundle/vim-windowswap/after,~/.vim/bundle/vim-autotag/after,~/.vim/bundle/tagbar/after
set shortmess=filnxtToOc
set showcmd
set sidescroll=1
set statusline=%f\ %h%m%r%=%9*\ DDDAdmin_test.cppcol:\ %c,
set suffixes=.bak,~,.swp,.o,.info,.aux,.log,.dvi,.bbl,.blg,.brf,.cb,.ind,.idx,.ilg,.inx,.out,.toc
set updatetime=50
let s:so_save = &so | let s:siso_save = &siso | set so=0 siso=0
let v:this_session=expand("<sfile>:p")
silent only
cd ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +1 testoutput
badd +335 DDDAdmin_test.cpp
badd +10 output
badd +63 makefile_ubuntu.mak
badd +30 ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/datadictionarycontrol.cpp
badd +19 ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/datadictionarycontrol.hpp
badd +1 \	bool\ CreateBlockFile(std::string\ filename);
badd +31 ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDArchitecture.txt
badd +1 \-
badd +1 term
badd +42 ~/.vimrc
badd +133 ~/GerritHub/CloudChat/install.sh
badd +1 .output.swp
badd +41 DDDAdmin_test.cpp(244):\ error\ in\ \"writeBlockIntoBFiStructure\":\ check\ f.chunk_id\ ==\ testfilename\ failed
badd +0 ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite/bash\ -\ 1
argglobal
silent! argdel *
edit ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd _ | wincmd |
split
1wincmd k
wincmd w
wincmd w
wincmd _ | wincmd |
split
1wincmd k
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd _ | wincmd |
split
1wincmd k
wincmd w
wincmd w
wincmd w
set nosplitbelow
set nosplitright
wincmd t
set winheight=1 winwidth=1
exe '1resize ' . ((&lines * 34 + 35) / 70)
exe 'vert 1resize ' . ((&columns * 79 + 119) / 238)
exe '2resize ' . ((&lines * 33 + 35) / 70)
exe 'vert 2resize ' . ((&columns * 79 + 119) / 238)
exe '3resize ' . ((&lines * 22 + 35) / 70)
exe 'vert 3resize ' . ((&columns * 79 + 119) / 238)
exe '4resize ' . ((&lines * 22 + 35) / 70)
exe 'vert 4resize ' . ((&columns * 79 + 119) / 238)
exe '5resize ' . ((&lines * 45 + 35) / 70)
exe 'vert 5resize ' . ((&columns * 78 + 119) / 238)
exe '6resize ' . ((&lines * 22 + 35) / 70)
exe 'vert 6resize ' . ((&columns * 158 + 119) / 238)
argglobal
let s:cpo_save=&cpo
set cpo&vim
imap <buffer> <MiddleMouse> <Plug>IMiddlemouse
imap <buffer> <LeftMouse> <Plug>ILeftmouse
nmap <buffer>  <Plug>NetrwHideEdit
nmap <buffer>  <Plug>NetrwRefresh
nnoremap <buffer> <F1> :he netrw-quickhelp
nmap <buffer> <silent> <Plug>Netrw2Leftmouse -
nmap <buffer> <2-LeftMouse> <Plug>Netrw2Leftmouse
nmap <buffer> <S-LeftDrag> <Plug>NetrwSLeftdrag
nmap <buffer> <S-LeftMouse> <Plug>NetrwSLeftmouse
nmap <buffer> <MiddleMouse> <Plug>NetrwMiddlemouse
nmap <buffer> <C-LeftMouse> <Plug>NetrwCLeftmouse
nmap <buffer> <LeftMouse> <Plug>NetrwLeftmouse
nmap <buffer> <nowait> <silent> <S-CR> <Plug>NetrwTreeSqueeze
nnoremap <buffer> <silent> <S-Up> :Pexplore
nnoremap <buffer> <silent> <S-Down> :Nexplore
let &cpo=s:cpo_save
unlet s:cpo_save
setlocal keymap=
setlocal noarabic
setlocal noautoindent
setlocal backupcopy=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=hide
setlocal nobuflisted
setlocal buftype=
setlocal nocindent
setlocal cinkeys=0{,0},0),:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=s1:/*,mb:*,ex:*/,://,b:#,:%,:XCOMM,n:>,fb:-
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t,i
setlocal concealcursor=
setlocal conceallevel=0
setlocal completefunc=youcompleteme#Complete
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal cursorline
setlocal define=
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal noexpandtab
if &filetype != 'netrw'
setlocal filetype=netrw
endif
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldtext=foldtext()
setlocal formatexpr=
setlocal formatoptions=tcq
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=0
setlocal include=
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
setlocal nolinebreak
setlocal nolisp
setlocal lispwords=
setlocal nolist
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal modeline
setlocal nomodifiable
setlocal nrformats=bin,octal,hex
set number
setlocal nonumber
setlocal numberwidth=4
setlocal omnifunc=youcompleteme#OmniComplete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal readonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal shiftwidth=8
setlocal noshortname
setlocal nosmartindent
setlocal softtabstop=0
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal statusline=
setlocal suffixesadd=
setlocal noswapfile
setlocal synmaxcol=3000
if &syntax != 'netrw'
setlocal syntax=netrw
endif
setlocal tabstop=33
setlocal tagcase=
setlocal tags=
setlocal textwidth=0
setlocal thesaurus=
setlocal noundofile
setlocal undolevels=-123456
setlocal nowinfixheight
setlocal nowinfixwidth
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let s:l = 8 - ((7 * winheight(0) + 17) / 34)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
8
normal! 0
lcd ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite
wincmd w
argglobal
edit ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite/output
setlocal keymap=
setlocal noarabic
setlocal noautoindent
setlocal backupcopy=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal nocindent
setlocal cinkeys=0{,0},0),:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=s1:/*,mb:*,ex:*/,://,b:#,:%,:XCOMM,n:>,fb:-
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t,i
setlocal concealcursor=nv
setlocal conceallevel=3
setlocal completefunc=youcompleteme#Complete
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal define=
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal noexpandtab
if &filetype != ''
setlocal filetype=
endif
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldtext=foldtext()
setlocal formatexpr=
setlocal formatoptions=tcq
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=0
setlocal include=
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
setlocal nolinebreak
setlocal nolisp
setlocal lispwords=
setlocal nolist
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal modeline
setlocal modifiable
setlocal nrformats=bin,octal,hex
set number
setlocal nonumber
setlocal numberwidth=4
setlocal omnifunc=
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal shiftwidth=8
setlocal noshortname
setlocal nosmartindent
setlocal softtabstop=0
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal statusline=
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != ''
setlocal syntax=
endif
setlocal tabstop=8
setlocal tagcase=
setlocal tags=
setlocal textwidth=0
setlocal thesaurus=
setlocal noundofile
setlocal undolevels=-123456
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
17,41fold
let s:l = 3 - ((2 * winheight(0) + 16) / 33)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
3
normal! 019|
lcd ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite
wincmd w
argglobal
edit ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/datadictionarycontrol.hpp
setlocal keymap=
setlocal noarabic
setlocal noautoindent
setlocal backupcopy=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=hide
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t,i
setlocal concealcursor=
setlocal conceallevel=0
setlocal completefunc=youcompleteme#Complete
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal define=
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal noexpandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldtext=foldtext()
setlocal formatexpr=
setlocal formatoptions=croql
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=0
setlocal include=
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
setlocal nolinebreak
setlocal nolisp
setlocal lispwords=
setlocal nolist
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal modeline
setlocal modifiable
setlocal nrformats=bin,octal,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=youcompleteme#OmniComplete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal shiftwidth=8
setlocal noshortname
setlocal nosmartindent
setlocal softtabstop=0
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal statusline=
setlocal suffixesadd=
setlocal noswapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=8
setlocal tagcase=
setlocal tags=
setlocal textwidth=0
setlocal thesaurus=
setlocal noundofile
setlocal undolevels=-123456
setlocal nowinfixheight
setlocal nowinfixwidth
setlocal wrap
setlocal wrapmargin=0
silent! normal! zE
let s:l = 42 - ((18 * winheight(0) + 11) / 22)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
42
normal! 0
lcd ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite
wincmd w
argglobal
edit ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite/DDDAdmin_test.cpp
setlocal keymap=
setlocal noarabic
setlocal noautoindent
setlocal backupcopy=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t,i
setlocal concealcursor=
setlocal conceallevel=0
setlocal completefunc=youcompleteme#Complete
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal define=
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal noexpandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldtext=foldtext()
setlocal formatexpr=
setlocal formatoptions=croql
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=0
setlocal include=
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
setlocal nolinebreak
setlocal nolisp
setlocal lispwords=
setlocal nolist
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal modeline
setlocal modifiable
setlocal nrformats=bin,octal,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=youcompleteme#OmniComplete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal shiftwidth=8
setlocal noshortname
setlocal nosmartindent
setlocal softtabstop=0
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal statusline=
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=8
setlocal tagcase=
setlocal tags=
setlocal textwidth=0
setlocal thesaurus=
setlocal noundofile
setlocal undolevels=-123456
setlocal nowinfixheight
setlocal nowinfixwidth
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
64,69fold
71,83fold
71,90fold
91,151fold
71
normal! zo
let s:l = 335 - ((10 * winheight(0) + 11) / 22)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
335
normal! 0
lcd ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite
wincmd w
argglobal
edit ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/datadictionarycontrol.cpp
setlocal keymap=
setlocal noarabic
setlocal noautoindent
setlocal backupcopy=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=
setlocal buflisted
setlocal buftype=
setlocal cindent
setlocal cinkeys=0{,0},0),:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,://
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t,i
setlocal concealcursor=
setlocal conceallevel=0
setlocal completefunc=youcompleteme#Complete
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal define=
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal noexpandtab
if &filetype != 'cpp'
setlocal filetype=cpp
endif
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldtext=foldtext()
setlocal formatexpr=
setlocal formatoptions=croql
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=0
setlocal include=
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
setlocal nolinebreak
setlocal nolisp
setlocal lispwords=
setlocal nolist
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal modeline
setlocal modifiable
setlocal nrformats=bin,octal,hex
set number
setlocal number
setlocal numberwidth=4
setlocal omnifunc=youcompleteme#OmniComplete
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal shiftwidth=8
setlocal noshortname
setlocal nosmartindent
setlocal softtabstop=0
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal statusline=
setlocal suffixesadd=
setlocal swapfile
setlocal synmaxcol=3000
if &syntax != 'cpp'
setlocal syntax=cpp
endif
setlocal tabstop=8
setlocal tagcase=
setlocal tags=
setlocal textwidth=0
setlocal thesaurus=
setlocal noundofile
setlocal undolevels=-123456
setlocal nowinfixheight
setlocal nowinfixwidth
set nowrap
setlocal nowrap
setlocal wrapmargin=0
silent! normal! zE
let s:l = 23 - ((16 * winheight(0) + 22) / 45)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
23
normal! 0
lcd ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite
wincmd w
argglobal
enew
file ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite/bash\ -\ 1
let s:cpo_save=&cpo
set cpo&vim
inoremap <buffer> <silent> <End> :py3 ConqueTerm_1.write(u("\x1bOF"))
inoremap <buffer> <silent> <Home> :py3 ConqueTerm_1.write(u("\x1bOH"))
inoremap <buffer> <silent> <Left> :py3 ConqueTerm_1.write(u("\x1b[D"))
inoremap <buffer> <silent> <Right> :py3 ConqueTerm_1.write(u("\x1b[C"))
inoremap <buffer> <silent> <Down> :py3 ConqueTerm_1.write(u("\x1b[B"))
inoremap <buffer> <silent> <Up> :py3 ConqueTerm_1.write(u("\x1b[A"))
inoremap <buffer> <silent> <S-Space> :py3 ConqueTerm_1.write(u(" "))
inoremap <buffer> <silent> <S-BS> :py3 ConqueTerm_1.write(u("\x08"))
inoremap <buffer> <silent> <BS> :py3 ConqueTerm_1.write(u("\x08"))
nnoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(3)
nnoremap <buffer> <silent> C :echo "Change mode disabled in shell."
nnoremap <buffer> <silent> P :py3 ConqueTerm_1.write_expr("@@")a
nnoremap <buffer> <silent> R :echo "Replace mode disabled in shell."
nnoremap <buffer> <silent> S :echo "Change mode disabled in shell."
nnoremap <buffer> <silent> [p :py3 ConqueTerm_1.write_expr("@@")a
nnoremap <buffer> <silent> ]p :py3 ConqueTerm_1.write_expr("@@")a
nnoremap <buffer> <silent> c :echo "Change mode disabled in shell."
nnoremap <buffer> <silent> p :py3 ConqueTerm_1.write_expr("@@")a
nnoremap <buffer> <silent> r :echo "Replace mode disabled in shell."
nnoremap <buffer> <silent> s :echo "Change mode disabled in shell."
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(1)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(2)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(3)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(4)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(5)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(6)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(7)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(8)
inoremap <buffer> <silent> 	 :py3 ConqueTerm_1.write_ord(9)
inoremap <buffer> <silent> <NL> :py3 ConqueTerm_1.write_ord(10)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(11)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(12)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(13)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(14)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(15)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(16)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(17)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(18)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(19)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(20)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(21)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(22)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(23)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(24)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(25)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(26)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(27)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(28)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(29)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(30)
inoremap <buffer> <silent>  :py3 ConqueTerm_1.write_ord(31)
inoremap <buffer> <silent>   :py3 ConqueTerm_1.write(u(" "))
inoremap <buffer> <silent> ! :py3 ConqueTerm_1.write_ord(33)
inoremap <buffer> <silent> " :py3 ConqueTerm_1.write_ord(34)
inoremap <buffer> <silent> # :py3 ConqueTerm_1.write_ord(35)
inoremap <buffer> <silent> $ :py3 ConqueTerm_1.write_ord(36)
inoremap <buffer> <silent> % :py3 ConqueTerm_1.write_ord(37)
inoremap <buffer> <silent> & :py3 ConqueTerm_1.write_ord(38)
inoremap <buffer> <silent> ' :py3 ConqueTerm_1.write_ord(39)
inoremap <buffer> <silent> ( :py3 ConqueTerm_1.write_ord(40)
inoremap <buffer> <silent> ) :py3 ConqueTerm_1.write_ord(41)
inoremap <buffer> <silent> * :py3 ConqueTerm_1.write_ord(42)
inoremap <buffer> <silent> + :py3 ConqueTerm_1.write_ord(43)
inoremap <buffer> <silent> , :py3 ConqueTerm_1.write_ord(44)
inoremap <buffer> <silent> - :py3 ConqueTerm_1.write_ord(45)
inoremap <buffer> <silent> . :py3 ConqueTerm_1.write_ord(46)
inoremap <buffer> <silent> / :py3 ConqueTerm_1.write_ord(47)
inoremap <buffer> <silent> 0 :py3 ConqueTerm_1.write_ord(48)
inoremap <buffer> <silent> 1 :py3 ConqueTerm_1.write_ord(49)
inoremap <buffer> <silent> 2 :py3 ConqueTerm_1.write_ord(50)
inoremap <buffer> <silent> 3 :py3 ConqueTerm_1.write_ord(51)
inoremap <buffer> <silent> 4 :py3 ConqueTerm_1.write_ord(52)
inoremap <buffer> <silent> 5 :py3 ConqueTerm_1.write_ord(53)
inoremap <buffer> <silent> 6 :py3 ConqueTerm_1.write_ord(54)
inoremap <buffer> <silent> 7 :py3 ConqueTerm_1.write_ord(55)
inoremap <buffer> <silent> 8 :py3 ConqueTerm_1.write_ord(56)
inoremap <buffer> <silent> 9 :py3 ConqueTerm_1.write_ord(57)
inoremap <buffer> <silent> : :py3 ConqueTerm_1.write_ord(58)
inoremap <buffer> <silent> ; :py3 ConqueTerm_1.write_ord(59)
inoremap <buffer> <silent> < :py3 ConqueTerm_1.write_ord(60)
inoremap <buffer> <silent> = :py3 ConqueTerm_1.write_ord(61)
inoremap <buffer> <silent> > :py3 ConqueTerm_1.write_ord(62)
inoremap <buffer> <silent> ? :py3 ConqueTerm_1.write_ord(63)
inoremap <buffer> <silent> @ :py3 ConqueTerm_1.write_ord(64)
inoremap <buffer> <silent> A :py3 ConqueTerm_1.write_ord(65)
inoremap <buffer> <silent> B :py3 ConqueTerm_1.write_ord(66)
inoremap <buffer> <silent> C :py3 ConqueTerm_1.write_ord(67)
inoremap <buffer> <silent> D :py3 ConqueTerm_1.write_ord(68)
inoremap <buffer> <silent> E :py3 ConqueTerm_1.write_ord(69)
inoremap <buffer> <silent> F :py3 ConqueTerm_1.write_ord(70)
inoremap <buffer> <silent> G :py3 ConqueTerm_1.write_ord(71)
inoremap <buffer> <silent> H :py3 ConqueTerm_1.write_ord(72)
inoremap <buffer> <silent> I :py3 ConqueTerm_1.write_ord(73)
inoremap <buffer> <silent> J :py3 ConqueTerm_1.write_ord(74)
inoremap <buffer> <silent> K :py3 ConqueTerm_1.write_ord(75)
inoremap <buffer> <silent> L :py3 ConqueTerm_1.write_ord(76)
inoremap <buffer> <silent> M :py3 ConqueTerm_1.write_ord(77)
inoremap <buffer> <silent> N :py3 ConqueTerm_1.write_ord(78)
inoremap <buffer> <silent> O :py3 ConqueTerm_1.write_ord(79)
inoremap <buffer> <silent> P :py3 ConqueTerm_1.write_ord(80)
inoremap <buffer> <silent> Q :py3 ConqueTerm_1.write_ord(81)
inoremap <buffer> <silent> R :py3 ConqueTerm_1.write_ord(82)
inoremap <buffer> <silent> S :py3 ConqueTerm_1.write_ord(83)
inoremap <buffer> <silent> T :py3 ConqueTerm_1.write_ord(84)
inoremap <buffer> <silent> U :py3 ConqueTerm_1.write_ord(85)
inoremap <buffer> <silent> V :py3 ConqueTerm_1.write_ord(86)
inoremap <buffer> <silent> W :py3 ConqueTerm_1.write_ord(87)
inoremap <buffer> <silent> X :py3 ConqueTerm_1.write_ord(88)
inoremap <buffer> <silent> Y :py3 ConqueTerm_1.write_ord(89)
inoremap <buffer> <silent> Z :py3 ConqueTerm_1.write_ord(90)
inoremap <buffer> <silent> [ :py3 ConqueTerm_1.write_ord(91)
inoremap <buffer> <silent> \ :py3 ConqueTerm_1.write_ord(92)
inoremap <buffer> <silent> ] :py3 ConqueTerm_1.write_ord(93)
inoremap <buffer> <silent> ^ :py3 ConqueTerm_1.write_ord(94)
inoremap <buffer> <silent> _ :py3 ConqueTerm_1.write_ord(95)
inoremap <buffer> <silent> ` :py3 ConqueTerm_1.write_ord(96)
inoremap <buffer> <silent> a :py3 ConqueTerm_1.write_ord(97)
inoremap <buffer> <silent> b :py3 ConqueTerm_1.write_ord(98)
inoremap <buffer> <silent> c :py3 ConqueTerm_1.write_ord(99)
inoremap <buffer> <silent> d :py3 ConqueTerm_1.write_ord(100)
inoremap <buffer> <silent> e :py3 ConqueTerm_1.write_ord(101)
inoremap <buffer> <silent> f :py3 ConqueTerm_1.write_ord(102)
inoremap <buffer> <silent> g :py3 ConqueTerm_1.write_ord(103)
inoremap <buffer> <silent> h :py3 ConqueTerm_1.write_ord(104)
inoremap <buffer> <silent> i :py3 ConqueTerm_1.write_ord(105)
inoremap <buffer> <silent> j :py3 ConqueTerm_1.write_ord(106)
inoremap <buffer> <silent> k :py3 ConqueTerm_1.write_ord(107)
inoremap <buffer> <silent> l :py3 ConqueTerm_1.write_ord(108)
inoremap <buffer> <silent> m :py3 ConqueTerm_1.write_ord(109)
inoremap <buffer> <silent> n :py3 ConqueTerm_1.write_ord(110)
inoremap <buffer> <silent> o :py3 ConqueTerm_1.write_ord(111)
inoremap <buffer> <silent> p :py3 ConqueTerm_1.write_ord(112)
inoremap <buffer> <silent> q :py3 ConqueTerm_1.write_ord(113)
inoremap <buffer> <silent> r :py3 ConqueTerm_1.write_ord(114)
inoremap <buffer> <silent> s :py3 ConqueTerm_1.write_ord(115)
inoremap <buffer> <silent> t :py3 ConqueTerm_1.write_ord(116)
inoremap <buffer> <silent> u :py3 ConqueTerm_1.write_ord(117)
inoremap <buffer> <silent> v :py3 ConqueTerm_1.write_ord(118)
inoremap <buffer> <silent> w :py3 ConqueTerm_1.write_ord(119)
inoremap <buffer> <silent> x :py3 ConqueTerm_1.write_ord(120)
inoremap <buffer> <silent> y :py3 ConqueTerm_1.write_ord(121)
inoremap <buffer> <silent> z :py3 ConqueTerm_1.write_ord(122)
inoremap <buffer> <silent> { :py3 ConqueTerm_1.write_ord(123)
inoremap <buffer> <silent> | :py3 ConqueTerm_1.write_ord(124)
inoremap <buffer> <silent> } :py3 ConqueTerm_1.write_ord(125)
inoremap <buffer> <silent> ~ :py3 ConqueTerm_1.write_ord(126)
let &cpo=s:cpo_save
unlet s:cpo_save
setlocal keymap=
setlocal noarabic
setlocal noautoindent
setlocal backupcopy=
setlocal nobinary
setlocal nobreakindent
setlocal breakindentopt=
setlocal bufhidden=hide
setlocal buflisted
setlocal buftype=nofile
setlocal nocindent
setlocal cinkeys=0{,0},0),:,0#,!^F,o,O,e
setlocal cinoptions=
setlocal cinwords=if,else,while,do,for,switch
setlocal colorcolumn=
setlocal comments=s1:/*,mb:*,ex:*/,://,b:#,:%,:XCOMM,n:>,fb:-
setlocal commentstring=/*%s*/
setlocal complete=.,w,b,u,t,i
setlocal concealcursor=nic
setlocal conceallevel=3
setlocal completefunc=youcompleteme#Complete
setlocal nocopyindent
setlocal cryptmethod=
setlocal nocursorbind
setlocal nocursorcolumn
setlocal nocursorline
setlocal define=
setlocal dictionary=
setlocal nodiff
setlocal equalprg=
setlocal errorformat=
setlocal noexpandtab
if &filetype != 'conque_term'
setlocal filetype=conque_term
endif
setlocal fixendofline
setlocal foldcolumn=0
setlocal foldenable
setlocal foldexpr=0
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldmarker={{{,}}}
setlocal foldmethod=manual
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldtext=foldtext()
setlocal formatexpr=
setlocal formatoptions=tcq
setlocal formatlistpat=^\\s*\\d\\+[\\]:.)}\\t\ ]\\s*
setlocal grepprg=
setlocal iminsert=0
setlocal imsearch=0
setlocal include=
setlocal includeexpr=
setlocal indentexpr=
setlocal indentkeys=0{,0},:,0#,!^F,o,O,e
setlocal noinfercase
setlocal iskeyword=@,48-57,_,192-255
setlocal keywordprg=
setlocal nolinebreak
setlocal nolisp
setlocal lispwords=
setlocal nolist
setlocal makeprg=
setlocal matchpairs=(:),{:},[:]
setlocal modeline
setlocal modifiable
setlocal nrformats=bin,octal,hex
set number
setlocal nonumber
setlocal numberwidth=4
setlocal omnifunc=
setlocal path=
setlocal nopreserveindent
setlocal nopreviewwindow
setlocal quoteescape=\\
setlocal noreadonly
setlocal norelativenumber
setlocal norightleft
setlocal rightleftcmd=search
setlocal noscrollbind
setlocal shiftwidth=8
setlocal noshortname
setlocal nosmartindent
setlocal softtabstop=0
setlocal nospell
setlocal spellcapcheck=[.?!]\\_[\\])'\"\	\ ]\\+
setlocal spellfile=
setlocal spelllang=en
setlocal statusline=
setlocal suffixesadd=
setlocal noswapfile
setlocal synmaxcol=3000
if &syntax != 'conque_term'
setlocal syntax=conque_term
endif
setlocal tabstop=8
setlocal tagcase=
setlocal tags=
setlocal textwidth=0
setlocal thesaurus=
setlocal noundofile
setlocal undolevels=-123456
setlocal nowinfixheight
setlocal nowinfixwidth
setlocal nowrap
setlocal wrapmargin=0
lcd ~/GerritHub/CloudChat/codeblocks_projects/distributeddatadictionary/DDDAdmin_testsuite
wincmd w
exe '1resize ' . ((&lines * 34 + 35) / 70)
exe 'vert 1resize ' . ((&columns * 79 + 119) / 238)
exe '2resize ' . ((&lines * 33 + 35) / 70)
exe 'vert 2resize ' . ((&columns * 79 + 119) / 238)
exe '3resize ' . ((&lines * 22 + 35) / 70)
exe 'vert 3resize ' . ((&columns * 79 + 119) / 238)
exe '4resize ' . ((&lines * 22 + 35) / 70)
exe 'vert 4resize ' . ((&columns * 79 + 119) / 238)
exe '5resize ' . ((&lines * 45 + 35) / 70)
exe 'vert 5resize ' . ((&columns * 78 + 119) / 238)
exe '6resize ' . ((&lines * 22 + 35) / 70)
exe 'vert 6resize ' . ((&columns * 158 + 119) / 238)
tabnext 1
if exists('s:wipebuf')
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 shortmess=filnxtToOc
let s:sx = expand("<sfile>:p:r")."x.vim"
if file_readable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &so = s:so_save | let &siso = s:siso_save
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
