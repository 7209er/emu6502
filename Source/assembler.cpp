#include "assembler.h"

Assembler::Assembler(std::string in){
    input = std::stringstream(in);
}

Assembler::~Assembler(){

}

std::string Assembler::convert(){
    while(!input.eof()){
        token.clear();
        input >> token;
        if (addressTable.find(token) != addressTable.end()){
            getAddressmode();
            switch(addressMode){
                case Implicit:
                    output << ("0x" + addressTable[token].Implicit + " ");
                break;
                case Accumulator:
                    output << ("0x" + addressTable[token].Accumulator + " ");
                break;
                case Immediate:
                    output << ("0x" + addressTable[token].Immediate + " " + value + " ");
                break;
                case ZeroPage:
                    output << ("0x" + addressTable[token].ZeroPage + " " + value + " ");
                break;
                case ZeroPageX:
                    output << ("0x" + addressTable[token].ZeroPageX + " " + value + " ");
                break;
                case ZeroPageY:
                    output << ("0x" + addressTable[token].ZeroPageY + " " + value + " ");
                break;
                case Relative:
                    output << ("0x" + addressTable[token].Relative + " " + value + " ");
                break;
                case Absolute:
                    output << ("0x" + addressTable[token].Absolute + " " + value + " ");
                break;
                case AbsoluteX:
                    output << ("0x" + addressTable[token].AbsoluteX + " " + value + " ");
                break;
                case AbsoluteY:
                    output << ("0x" + addressTable[token].AsboluteY + " " + value + " ");
                break;
                case Indirect:
                    output << ("0x" + addressTable[token].Indirect + " " + value + " ");
                break;
                case IndirectX:
                    output << ("0x" + addressTable[token].IndirectX + " " + value + " ");
                break;
                case IndirectY:
                    output << ("0x" + addressTable[token].IndirectY + " " + value + " ");
                break;
            };
        }
        else{
            throw std::invalid_argument{"Invalid operation: " + token};
        }
    }

    std::string temp = output.str();
    temp.pop_back();    // Deletes the whitespace at the end
    return temp;
}

void Assembler::getAddressmode(){
    // Implicit and Relative mode always have to be used if available
    if(!addressTable[token].Implicit.empty()){
        addressMode = Implicit;
        return;
    }

    nextToken.clear();
    input >> nextToken;

    if(!addressTable[token].Relative.empty()){
        addressMode = Relative;
        if(nextToken[0] == '$' & isHex(nextToken.substr(1)) && nextToken.size() == 3)
            value = "0x" + nextToken.substr(1);
        else
            throw std::invalid_argument{"Invalid value in combination with Relative: " + nextToken};
    }

    else if(nextToken[0] == '#' && isHex(nextToken.substr(1)) && nextToken.size() == 3){
        addressMode = Immediate;
        value = "0x" + nextToken.substr(1);
    }

    else if(nextToken[0] == 'A' && nextToken.size() == 1){
        addressMode = Accumulator;
        value.clear();
    }

    else if(nextToken[0] == '$'){
        if(hasSuffix(nextToken, ",X") && isHex(nextToken.substr(1, nextToken.size()-3))){
            if(nextToken.size() == 5){
                addressMode = ZeroPageX;
                value = "0x" + nextToken.substr(1,2);
            }
            else if(nextToken.size() == 7){
                addressMode = AbsoluteX;
                value = "0x" + nextToken.substr(3,2) + " 0x" + nextToken.substr(1,2);
            }
            else{
                throw std::invalid_argument{"Invalid value: A" + nextToken};
            }
        }

        else if(hasSuffix(nextToken, ",Y") && isHex(nextToken.substr(1, nextToken.size()-3))){
            if(nextToken.size() == 5){
                addressMode = ZeroPageY;
                value = "0x" + nextToken.substr(1,2);
            }
            else if(nextToken.size() == 7){
                addressMode = AbsoluteY;
                value = "0x" + nextToken.substr(3,2) + " 0x" + nextToken.substr(1,2);
            }
            else{
                throw std::invalid_argument{"Invalid value: B" + nextToken};
            }
        }

        else if(isHex(nextToken.substr(1))){
            if(nextToken.size() == 5){
                addressMode = Absolute;
                value = "0x" + nextToken.substr(3,2) + " 0x" + nextToken.substr(1,2);
            }
            else if(nextToken.size() == 3){
                addressMode = ZeroPage;
                value = "0x" + nextToken.substr(1);
            }
            else{
                throw std::invalid_argument{"Invalid value: C" + nextToken};
            }
        }
    }

    else if(nextToken.substr(0,2) == "($"){
        if(hasSuffix(nextToken, ",X)") && nextToken.size() == 7 && isHex(nextToken.substr(2,2))){
            addressMode = IndirectX;
            value = "0x" + nextToken.substr(2,2);
        }
        else if(hasSuffix(nextToken, "),Y") && nextToken.size() == 7 && isHex(nextToken.substr(2,2))){
            addressMode = IndirectY;
            value = "0x" + nextToken.substr(2,2);
        }
        else if(hasSuffix(nextToken, ")") && nextToken.size() == 5 && isHex(nextToken.substr(2,2))){
            addressMode = Indirect;
            value = "0x" + nextToken.substr(2,2);
        }
        else{
            throw std::invalid_argument{"Invalid value with combination Indirect/X/Y" + nextToken};
        }
    }

    else{
        throw std::invalid_argument{"Invalid value: D" + nextToken};
    }
}


bool Assembler::isHex(std::string inputStr){
    static std::string hexDigits = "0123456789ABCDEFabcdef";
    for(char x : inputStr){
        if(hexDigits.find(x) == std::string::npos)
            return false;
    }
    return true;
}

bool Assembler::hasSuffix(std::string inputStr, std::string suffix){
    if(inputStr.substr(inputStr.size() - suffix.size()) == suffix)
        return true;
    return false;
}