#include "../../include/csl.h"

CaseNode *
_CaseNode_New ( uint64 type, block block, int64 value )
{
    CaseNode * cnode = ( CaseNode* ) Mem_Allocate ( sizeof ( CaseNode ), type ) ;
    cnode->CN_CaseBlock = ( uint64 ) block ;
    cnode->CN_CaseUint64Value = value ; //nb. CN_CaseUint64Value is member of a union 
    return cnode ;
}

void
Match_MapFunction ( dlnode * node, uint64 switchValue )
{
    CaseNode * cnode = ( CaseNode* ) node ;
    if ( String_Equal ( cnode->CN_CaseBytePtrValue, ( byte* ) switchValue ) )
        ( ( block ) ( cnode->CN_CaseBlock ) ) ( ) ;
}

void
MatchAccessFunction ( )
{
    dllist_Map1 ( ( dllist* ) TOS, ( MapFunction1 ) Match_MapFunction, _Dsp_ [ - 1 ] ) ;
    DataStack_DropN ( 2 ) ;
}

void
_CSL_Case ( uint64 allocType )
{
    block caseBlock = ( block ) TOS ;
    int64 caseValue = ( uint64 ) String_New ( ( byte* ) NOS, STRING_MEM ) ;
    DataStack_DropN ( 2 ) ;
    if ( ! _Compiler_->CurrentMatchList ) _Compiler_->CurrentMatchList = _dllist_New ( allocType ) ;
    CaseNode * cnode = _CaseNode_New ( allocType, caseBlock, caseValue ) ;
    dllist_AddNodeToTail ( _Compiler_->CurrentMatchList, ( dlnode* ) cnode ) ;
}

void
CSL_Case ( )
{
    _CSL_Case ( DICTIONARY ) ;
}
 
void
_CSL_Match ( uint64 allocType )
{
    if ( ! _Compiler_->CurrentMatchList ) _Compiler_->CurrentMatchList = _dllist_New ( allocType ) ;
    if ( CompileMode )
    {
        _Do_LiteralValue ( ( int64 ) _Compiler_->CurrentMatchList ) ;
        Compile_Call_TestRSP ( ( byte* ) MatchAccessFunction ) ;
    }
    else
    {
        dllist_Map1 ( _Compiler_->CurrentMatchList, ( MapFunction1 ) Match_MapFunction, TOS ) ;
        DataStack_DropN ( 1 ) ;
    }
    _Compiler_->CurrentMatchList = 0 ; // this allows no further "case"s to be added to this "switch" list a new list will be started with the next "case"
}

void
CSL_Match ( )
{
    _CSL_Match ( DICTIONARY ) ;
}

void
_CS_Case ( uint64 allocType )
{
    _Interpret_Until_Token ( _Interpreter_, ";", 0 ) ;
    block caseBlock = ( block ) TOS ;
    int64 caseValue = ( uint64 ) String_New ( ( byte* ) NOS, STRING_MEM ) ;
    DataStack_DropN ( 2 ) ;
    if ( ! _Compiler_->CurrentMatchList ) _Compiler_->CurrentMatchList = _dllist_New ( allocType ) ;
    CaseNode * cnode = _CaseNode_New ( allocType, caseBlock, caseValue ) ;
    dllist_AddNodeToTail ( _Compiler_->CurrentMatchList, ( dlnode* ) cnode ) ;
}

void
CS_Case ( )
{
    _CS_Case ( DICTIONARY ) ;
}

void
_CS_Match ( uint64 allocType )
{
    if ( ! _Compiler_->CurrentMatchList ) _Compiler_->CurrentMatchList = _dllist_New ( allocType ) ;
    Interpreter * interp = _Interpreter_ ;
    byte * token = Lexer_ReadToken ( interp->Lexer0 ) ;
    Word * word = _Interpreter_TokenToWord ( interp, token, -1, -1 ) ;
    //CSL_Interpret_C_Blocks ( 2, 0, 0 ) ;
    _Interpret_Until_Token ( _Interpreter_, ";", 0 ) ;
    Interpreter_DoWord ( interp, word, -1, -1 ) ;
    if ( CompileMode )
    {
        _Do_LiteralValue ( ( int64 ) _Compiler_->CurrentMatchList ) ;
        Compile_Call_TestRSP ( ( byte* ) MatchAccessFunction ) ;
    }
    else
    {
        dllist_Map1 ( _Compiler_->CurrentMatchList, ( MapFunction1 ) Match_MapFunction, TOS ) ;
        DataStack_DropN ( 1 ) ;
    }
    _Compiler_->CurrentMatchList = 0 ; // this allows no further "case"s to be added to this "switch" list a new list will be started with the next "case"
    //CSL_BlockRun ( ) ;
}

void
CS_Match ( )
{
    _CS_Match ( DICTIONARY ) ;
}

