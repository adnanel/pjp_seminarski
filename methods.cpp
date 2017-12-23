
bool Assignable(int position) {
    std::wstring name = GetNodeName(position);

    return name == std::wstring(L"VAR") ||
           name == std::wstring(L"INDEX") ||
           name == std::wstring(L"PTR");
}

bool IsDeclaredLocal(Name name1)
{  std::wstring name=std::wstring(name1);
    std::string::size_type found;
    std::wstring decls[4]={L"EXTRN",L"LARRDEF",L"LVARDEF",L"FPARAM"};
    for (int i=0;i<4;i++) {
        found = ParseList.find(L"("+decls[i]+L" "+name+L" ",scopepos);
        if (found!=std::string::npos)
            return true;
    }
    return false;
}

bool IsAuto(Name name1)
{  std::wstring name=std::wstring(name1);
    std::string::size_type found;
    std::wstring decls[3]={L"LARRDEF",L"LVARDEF",L"FPARAM"};
    for (int i=0;i<3;i++) {
        found = ParseList.find(L"("+decls[i]+L" "+name+L" ",scopepos);
        if (found!=std::string::npos)
            return true;
    }
    return false;
}

bool IsDeclaredGlobal(Name name1)
{
    std::wstring name=std::wstring(name1);
    std::string::size_type found;
    std::wstring decls[4]={L"GARRDEF",L"GVARDEF",L"FUNCDEF"};
    for (int i=0;i<4;i++) {
        found = ParseList.find(L"("+decls[i]+L" "+name+L" ",0);
        if (found!=std::string::npos)
            return true;
    }
   
    return false;
}

void UndecErr  (Name name)
{
    Name msg;
    wcscpy(msg,L"Undeclared ");
    wcscat(msg,name);
    SemErr (msg);
}

void AlreadyErr  (Name name)
{
    Name msg;
    wcscpy(msg,L"Already declared ");
    wcscat(msg,name);
    SemErr (msg);
}

std::wstring GetNodeName (int position)
{
    wstring NodeName;
    NodeName=L"";
    if (ParseList[position] != '(')
        return L"";
    position++;
    while (ParseList[position] >='A' && ParseList[position] <='Z' )
    {  NodeName+=ParseList[position];
        position++;
    }
    return NodeName;
}

std::wstring GetElemName (int position, int elemnum)
{
    wstring ElemName;
    position=ElemPos(position,elemnum);
    ElemName=L"";
    if (position>0)
      while (ParseList[position] !=' ' && ParseList[position] !=')' )
      { 
          ElemName+=ParseList[position];
          position++;
      }
    return ElemName;
}

int ElemPos(int position, int elemnum)
{
    int currel,brlevel;
    currel=0;
    position++;
    while (ParseList[position] >='A' && ParseList[position] <='Z' )
        position++;
    for (;;)
    {
        while (ParseList[position] ==' ' )
            position++;
        currel++;
        if (currel==elemnum && ParseList[position] !=')')
            return position;
        switch (ParseList[position])
        {
            case '(':
                brlevel=1;
                position++;
                while (brlevel !=0)
                {
                    while (ParseList[position] !='(' && ParseList[position] !='`'  && ParseList[position] !=')')
                        position++;
                    switch(ParseList[position])
                    {
                        case '(':
                            brlevel++;position++;
                            break;
                        case ')':
                            brlevel--;position++;
                            break;
                        case '`':
                            do {position++;} while (ParseList[position]!='`');position++;
                            break;
                    }
                }
                break;
            case ')':
                return -1;
                break;
            case '`':
                do
                {
                    position++;
                   
                } while (ParseList[position]!='`');
                position++;
                break;
            default:
                while (ParseList[position] !='(' && ParseList[position] !=' '  && ParseList[position] !=')')
                  position++;
        }
    }
   
}





std::map< std::wstring, std::vector< std::wstring > > FunctionParameters;
std::map< std::wstring, std::vector< std::wstring > > FunctionLocals;

std::wstring CurrentFunction;

void Compile(int position, std::wstringstream& data, std::wstringstream& bss, std::wstringstream& text)
{
    wstring nodename,elemname;
    int par,z,q;
    nodename=GetNodeName(position);
    par=1;
    if (nodename== L"ADD") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" ADD EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" ADD EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" ADD EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"ADDMOV") {
        // todo
        text<<"TODO - ADDMOV" << endl;
    }
    else if (nodename== L"ADDROF") {
        // todo
        text<<"TODO - ADDROF" << endl;
    }
    else if (nodename== L"AND") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" AND EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" AND EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" AND EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"ANDMOV") {
        // todo
        text<<"TODO - ANDMOV" << endl;
    }
    else if (nodename== L"ARG") {
        // todo
        text<<"TODO - ARG" << endl;
    }
    else if (nodename== L"ASIZE") {
        // todo
        text<<"TODO - ASIZE" << endl;
    }
    else if (nodename== L"BLOCK") {

        while ((z=ElemPos(position,par)) >0)
        {
          Compile(z, data, bss, text);;
          par++;
        }
    }
    else if (nodename== L"BREAK") {
        // todo
        text<<"TODO - BREAK" << endl;
    }
    else if (nodename== L"B" ) {
        while ((z=ElemPos(position,par)) >0)
        {
          Compile(z, data, bss, text);;
          par++;
        }
    }
    else if (nodename== L"CHAR") {
        // todo
        text<<"TODO - CHAR" << endl;
    }
    else if (nodename== L"CONDEXPR") {
        // todo
        text<<"TODO - CONDEXPR" << endl;
    }
    else if (nodename== L"CONTINUE") {
        // todo
        text<<"TODO - CONTINUE" << endl;
    }
    else if (nodename== L"DECLSTAT") {
        while ((z=ElemPos(position,par)) >0)
        {
          Compile(z, data, bss, text);;
          par++;
        }
    }
    else if (nodename== L"DEFAULT") {
        // todo
        text<<"TODO - DEFAULT" << endl;
    }
    else if (nodename== L"DIV") {
        // todo
        text<<"TODO - DIV" << endl;
    }
    else if (nodename== L"DIVMOV") {
        // todo
        text<<"TODO - DIVMOV" << endl;
    }
    else if (nodename== L"EQU") {
        // todo
        text<<"TODO - EQU" << endl;
    }
    else if (nodename== L"EXTRN") {
        // todo
        // text<<"TODO - EXTRN" << endl;
    }
    else if (nodename== L"FHEADER") {
        elemname = GetElemName(position, 1);

        while ((z=ElemPos(position,par)) >0)
        {
            Compile(z, data, bss, text);;
            par++;
        }
    }
    else if (nodename== L"FPARAM") {
        elemname = GetElemName(position, 1);

        FunctionParameters[CurrentFunction].push_back(elemname);
    }
    else if (nodename== L"FUNCCALL") {
        // todo
        text<<"TODO - FUNCCALL" << endl;
    }
    else if (nodename== L"FUNCDEF") {
        elemname=GetElemName(position,1);
        CurrentFunction = elemname;

        text << L" GLOBAL "<< elemname<< endl;
        text << endl << elemname<<":" << endl;
        text << L" PUSH EBP"<<endl<< L" MOV EBP,ESP"<<endl << L" SUB ESP,"<<elemname<<L"_len"<<endl;

        Compile (ElemPos(position,2), data, bss, text);
        Compile (ElemPos(position,3), data, bss, text);
        text << L" MOV ESP,EBP" << endl << L" RET " << endl;

        data << elemname << "_len db " << (FunctionParameters[CurrentFunction].size() * 4) << endl;
    }
    else if (nodename== L"GARRDEF") {
        // todo
        text<<"TODO - GARRDEF" << endl;
    }
    else if (nodename== L"GOTO") {
        // todo
        text<<"TODO - GOTO" << endl;
    }
    else if (nodename== L"GREATEREQUTHAN") {
        // todo
        text<<"TODO - GREATEREQUTHAN" << endl;
    }
    else if (nodename== L"GREATERTHAN") {
        // todo
        text<<"TODO - GREATERTHAN" << endl;
    }
    else if (nodename== L"GVARDEF") {
        elemname = GetElemName(position, 1);

        bss << elemname << " resd 1" << endl;
    }
    else if (nodename== L"IFELSE") {
        // todo
        text<<"TODO - IFELSE" << endl;
    }
    else if (nodename== L"IF") {
        // todo
        text<<"TODO - IF" << endl;
    }
    else if (nodename== L"INDEX") {
        // todo
        text<<"TODO - INDEX" << endl;
    }
    else if (nodename== L"INIT") {
        // todo
        text<<"TODO - INIT" << endl;
    }
    else if (nodename== L"INT") {
        text << L" MOV EAX,"<< GetElemName(position,1)<< endl ;
    }
    else if (nodename== L"ISEQUMOV") {
        // todo
        text<<"TODO - ISEQUMOV" << endl;
    }
    else if (nodename== L"ISGREATEREQUMOV") {
        // todo
        text<<"TODO - ISGREATEREQUMOV" << endl;
    }
    else if (nodename== L"ISGREATERMOV") {
        // todo
        text<<"TODO - ISGREATERMOV" << endl;
    }
    else if (nodename== L"ISLESSEQUMOV") {
        // todo
        text<<"TODO - ISLESSEQUMOV" << endl;
    }
    else if (nodename== L"ISLESSMOV") {
        // todo
        text<<"TODO - ISLESSMOV" << endl;
    }
    else if (nodename== L"ISNEQUMOV") {
        // todo
        text<<"TODO - ISNEQUMOV" << endl;
    }
    else if (nodename== L"LABEL") {
        // todo
        text<<"TODO - LABEL" << endl;
    }
    else if (nodename== L"LARRDEF") {
        // todo
        text<<"TODO - LARRDEF" << endl;
    }
    else if (nodename== L"LESSEQUTHAN") {
        // todo
        text<<"TODO - LESSEQUTHAN" << endl;
    }
    else if (nodename== L"LESSTHAN") {
        // todo
        text<<"TODO - LESSTHAN" << endl;
    }
    else if (nodename== L"LSHIFT") {
        // todo
        text<<"TODO - LSHIFT" << endl;
    }
    else if (nodename== L"LSHIFTMOV") {
        // todo
        text<<"TODO - LSHIFTMOV" << endl;
    }
    else if (nodename== L"LVARDEF") {
        elemname = GetElemName(position, 1);

        // todo
        text<<"TODO - LVARDEF - " << elemname << endl;
    }
    else if (nodename== L"MOD") {
        // todo
        text<<"TODO - MOD" << endl;
    }
    else if (nodename== L"MODMOV") {
        // todo
        text<<"TODO - MODMOV" << endl;
    }
    else if (nodename== L"MOV") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        if (GetNodeName(z)==L"VAR")
        {
            if (GetNodeName(q)==L"INT")
            {
              text << L" MOV DWORD [" << GetElemName(z,1)<<"],"<< GetElemName(q,1)<< endl ;
            }
            else
            if (GetNodeName(q)==L"VAR")
            {
              text << L" MOV EAX, [" << GetElemName(q,1)<<"]"<< endl;
              text << L" MOV [" << GetElemName(z,1)<<"],EAX"<< endl;
            }
            else
            {
              Compile(q, data, bss, text);;
              text << L" MOV [" << GetElemName(z,1)<<"],EAX"<< endl;
            }
        }
        else
        {
            if (GetNodeName(q)==L"INT")
            {
              Compile(z, data, bss, text);;
              text << L" MOV DWORD [EBX],"<< GetElemName(q,1)<< endl ;
            }
            else
            if (GetNodeName(q)==L"VAR")
            {
              Compile(z, data, bss, text);;
              text << L" MOV EAX, [" << GetElemName(q,1)<<"]"<< endl;
              text << L" MOV [EBX],EAX"<< endl;
            }
            else
            {
              Compile(q, data, bss, text);;
              text << L" PUSH EAX"<< endl;
              Compile(z, data, bss, text);;
              text << L" POP EAX"<< endl;
              text << L" MOV [EBX],EAX"<< endl;
            }


        }
    }
    else if (nodename== L"MUL") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" MUL EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" MUL EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" MUL EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"MULTMOV") {
        // todo
        text<<"TODO - MULTMOV" << endl;
    }
    else if (nodename== L"NEQU") {
        // todo
        text<<"TODO - NEQU" << endl;
    }
    else if (nodename== L"ONUMBER") {
        // todo
        text<<"TODO - ONUMBER" << endl;
    }
    else if (nodename== L"OR") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" OR EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" OR EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" OR EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"ORMOV") {
        // todo
        text<<"TODO - ORMOV" << endl;
    }
    else if (nodename== L"POSTDEC") {
        // todo
        text<<"TODO - POSTDEC" << endl;
    }
    else if (nodename== L"POSTINC") {
        // todo
        text<<"TODO - POSTINC" << endl;
    }
    else if (nodename== L"PREDEC") {
        // todo
        text<<"TODO - PREDEC" << endl;
    }
    else if (nodename== L"PREINC") {
        // todo
        text<<"TODO - PREINC" << endl;
    }
    else if (nodename== L"PTR") {
        // todo
        text<<"TODO - PTR" << endl;
    }
    else if (nodename== L"RETURN") {
        // todo
        text<<"TODO - RETURN" << endl;
    }
    else if (nodename== L"RETURNPARAM") {
        // todo
        text<<"TODO - RETURNPARAM" << endl;
    }
    else if (nodename== L"RSHIFT") {
        // todo
        text<<"TODO - RSHIFT" << endl;
    }
    else if (nodename== L"RSHIFTMOV") {
        // todo
        text<<"TODO - RSHIFTMOV" << endl;
    }
    else if (nodename== L"SAMEAS") {
        // todo
        text<<"TODO - SAMEAS" << endl;
    }
    else if (nodename== L"STRING") {
        // todo
        text<<"TODO - STRING" << endl;
    }
    else if (nodename== L"SUB") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" SUB EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" SUB EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" SUB EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"SUBMOV") {
        // todo
        text<<"TODO - SUBMOV" << endl;
    }
    else if (nodename== L"SWITCH") {
        // todo
        text<<"TODO - SWITCH" << endl;
    }
    else if (nodename== L"UMINUS") {
        // todo
        text<<"TODO - UMINUS" << endl;
    }
    else if (nodename== L"UNEG") {
        // todo
        text<<"TODO - UNEG" << endl;
    }
    else if (nodename== L"UNOT") {
        // todo
        text<<"TODO - UNOT" << endl;
    }
    else if (nodename== L"UPLUS") {
        // todo
        text<<"TODO - UPLUS" << endl;
    }
    else if (nodename== L"VAR") {
        text << L" MOV EAX, [" << GetElemName(position,1)<<"]"<< endl;
    }
    else if (nodename== L"WHILE") {
        // todo
        text<<"TODO - WHILE" << endl;
    }
    else if (nodename== L"XOR") {
        z=ElemPos(position,1);
        q=ElemPos(position,2);
        Compile(z, data, bss, text);;
        if (GetNodeName(q)==L"INT")
        {
              text << L" XOR EAX,"<< GetElemName(q,1)<< endl ;
        }
        else
        if (GetNodeName(q)==L"VAR")
        {
              text << L" XOR EAX,[" << GetElemName(q,1)<<"]"<< endl;
        }
        else
        {
            text << L" PUSH EAX";
            Compile(q, data, bss, text);;
            text << L" POP EBX";
            text << L" XOR EAX,EBX"<< endl;
        }
    }
    else if (nodename== L"XORMOV ") {
        // todo
        text<<"TODO - XORMOV" << endl;
    }
}