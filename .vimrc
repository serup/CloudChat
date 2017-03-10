set nocompatible              " be iMproved, required
filetype off                  " required

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'

" The following are examples of different formats supported.
" Keep Plugin commands between vundle#begin/end.
" plugin on GitHub repo
Plugin 'tpope/vim-fugitive'
" The sparkup vim script is in a subdirectory of this repo called vim.
" Pass the path to set the runtimepath properly.
Plugin 'rstacruz/sparkup', {'rtp': 'vim/'}
" Install YCM
Plugin 'Valloric/YouCompleteMe'
" Install vim startify
Plugin 'mhinz/vim-startify'
" Install vim-windowswap
Plugin 'wesQ3/vim-windowswap'
" Install vim-autotag
Plugin 'craigemery/vim-autotag'
" Install tagbar
Plugin 'majutsushi/tagbar'
" Install ConqueGdb
Plugin 'vim-scripts/Conque-GDB'
" Install nerdtree
Plugin 'scrooloose/nerdtree'
" Install nerdtree git plugin
Plugin 'Xuyuanp/nerdtree-git-plugin'
" Install vim-maktaba plugin for plugin developers - used in foldcol
Plugin 'google/vim-maktaba'
" Install foldcol - folding columns using <ctrl-v> visual mark, then :VFoldCol  
Plugin 'paulhybryant/foldcol'
" Install vim-ctrlspace
Plugin 'vim-ctrlspace/vim-ctrlspace'

" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required
" To ignore plugin indent changes, instead use:
"filetype plugin on
"
" Brief help
" :PluginList       - lists configured plugins
" :PluginInstall    - installs plugins; append `!` to update or just :PluginUpdate
" :PluginSearch foo - searches for foo; append `!` to refresh local cache
" :PluginClean      - confirms removal of unused plugins; append `!` to auto-approve removal
"
" see :h vundle for more details or wiki for FAQ
" Put your non-Plugin stuff after this line
"set mouse+=r
set mouse=a
"if &term =~ '^screen'
" tmux knows the extended mouse mode
	set ttymouse=xterm2
"endif
noremap <C-W>M <C-W>\| <C-W>_
noremap <C-W>m <C-W>=
"execute pathogen#infect()
syntax on


let mapleader=","
let g:windowswap_map_keys = 0 "prevent default bindings
vmap <leader>Y :w! ~/tmp/vitmp<CR>
nmap <leader>P :r! cat ~/tmp/vitmp<CR>
nnoremap <2-LeftMouse> * 
nnoremap <RightMouse> * 
nnoremap <silent> <leader>yw :call WindowSwap#MarkWindowSwap()<CR>
nnoremap <silent> <leader>pw :call WindowSwap#DoWindowSwap()<CR>
nnoremap <silent> <leader>ww :call WindowSwap#EasyWindowSwap()<CR>
noremap <F2> :AnsiEsc<CR>
noremap <F9> :so build_project.vim<CR>:%g/^{/normal! zf%<CR>:set foldmethod=marker<CR>gg<CR>
noremap <F3> :so test_project.vim <CR>:%g/^{/normal! zf%<CR>:set foldmethod=marker<CR>gg<CR>
noremap <silent> <leader>ct :!ctags-proj.sh<CR>
noremap <silent> <leader>fa :%g/^{/normal! zf%<CR>
noremap <silent> <leader>jf <C-]><CR>
noremap <silent> <leader>jb <C-t><CR>
noremap <silent> <leader>tb :TagbarToggle<CR>
noremap <silent> <leader>ft :NERDTreeToggle<CR>
noremap <silent> <leader>gb :Gblame<CR>
noremap <silent> <leader>ld :Linediff<CR>
noremap <silent> <leader>lc `.
noremap <silent> <leader>li `^
noremap <silent> <leader>db :ConqueGdb<CR>
noremap <silent> <leader>nw :set nowrap<CR>
noremap <silent> <leader>vzf :VFoldCol<CR>
noremap <silent> <leader>vzr :VFoldClear<CR>
nnoremap <leader>xml :so xml_project_output.vim<CR>
noremap <leader>M <C-W>\| <C-W>_
noremap <leader>m <C-W>=
noremap <silent> <leader>nt :tabn<CR>   
noremap <silent> <leader>pt :tabp<CR>
noremap <silent> <leader>nt :tabnew<CR>
map <C-l> :tabn<CR>
map <C-h> :tabp<CR>
map <C-n> :tabnew<CR>
"noremap <F3> :echo 'Current time is ' . strftime('%c')<CR>
"nnoremap <silent> <F2> :lchdir %:p:h<CR>:pwd<CR>
set showcmd
set sessionoptions+=folds
set statusline=%f\ line:\ %l,%c
augroup XML
	autocmd!
	autocmd FileType xml setlocal foldmethod=indent foldlevelstart=999 foldminlines=0
augroup END
set autoread	
set term=screen-256color
colorscheme desert
set foldnestmax=8
"set foldmethod=marker

if exists('$TMUX')
	autocmd BufEnter * call system("tmux rename-window " . expand("%:t"))
	autocmd VimLeave * call system("tmux rename-window bash")
      	autocmd BufEnter * let &titlestring = ' ' . expand("%:t")
	set title
endif

" use C-v to mark a column of numbers, then use C-a to increment them
function! Incr()
	let a = line('.') - line("'<")
	let c = virtcol("'<")
	if a > 0
		execute 'normal! '.c.'|'.a."\<C-a>"
	endif
	normal `<
endfunction
vnoremap <C-a> :call Incr()<CR>


let g:ConqueTerm_Color = 2         " 1: strip color after 200 lines, 2: always with color
let g:ConqueTerm_CloseOnEnd = 1    " close conque when program ends running
let g:ConqueTerm_StartMessages = 0 " display warning messages if conqueTerm is configured incorrectly 

"let g:ycm_global_ycm_extra_conf = "~/.vim/.ycm_extra_conf.py"
let g:ycm_global_ycm_extra_conf = "~/.vim/.bundle/YouCompleteMe/third_party/ycmd/examples/.ycm_extra_conf.py"
let g:ycm_disable_for_files_larger_than_kb = 1000000

function! s:DiffWithSaved()
	let filetype=&ft
	diffthis
	vnew | r # | normal! 1Gdd
	diffthis
	exe "setlocal bt=nofile bh=wipe nobl noswf ro ft=" . filetype
endfunction
com! DiffSaved call s:DiffWithSaved()

:command DiffOrig rightbelow vertical new | set bt=nofile | r # | 0d_ | diffthis | wincmd p | diffthis

" used with vim-ctrlspace
set nocompatible
set hidden
set showtabline=0
let g:CtrlSpaceHeight = 5

"disable syntastic on the statusline
let g:statline_syntastic = 0
let @m = 'iBOOST_LOG_MESSAGE( " " );'
let @q = 'iBOOST_AUTO_TEST_CASE(x){cout << "BOOST_AUTO_TEST_CASE(x)\n{" << endl;BOOST_CHECK(true == false);cout << "}" << endl;}'
let @t = 'iBOOST_TEST_MESSAGE(" " );'
let @j = 'y/^R"^M'
let @h = '!xmllint --format - | highlight --out-format=ansi --syntax=xml'
let @W = '!curl wttr.in/copenhagen?lang=da'
set autoindent noexpandtab tabstop=4 shiftwidth=4
"make sure color codes are enabled at startup
"autocmd VimEnter * AnsiEsc

