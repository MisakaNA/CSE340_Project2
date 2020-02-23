#ifndef PROGRAM_PROJECT2_H
#define PROGRAM_PROJECT2_H

#include "lexer.h"

//void parse_Rule_list();
//void parse_Rule();
//void parse_Right_hand_side();
//void parse_Id_list();

struct Rule{
    std::string lhs;
    std::vector<std::string> rhs;
};





#endif //PROGRAM_PROJECT2_H
