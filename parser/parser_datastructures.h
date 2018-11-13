#ifndef PARSER_DATASTRUCTURES_H_INCLUDED
#define PARSER_DATASTRUCTURES_H_INCLUDED


struct Token
{
    int type;
    int n_value;
    char s_value[STRING_VALUE_SIZE];
};

struct ASTnode
{
    int type;
    int n_value;
    int child_count;
    int max_childs;

    char s_value[STRING_VALUE_SIZE];
    ASTnode** childs;

    ASTnode()
    {
        this->child_count = 0;
        this->max_childs = MAX_CHILD_1;
        childs = new ASTnode*[max_childs];
    }

    void push_child(ASTnode *ptr)
    {
        if (child_count == max_childs)
        {
            this->max_childs += 10;
            ASTnode **tmp = (ASTnode**)realloc(this->childs, this->max_childs*sizeof(ASTnode*));
            if (tmp != NULL)
            {
                this->childs = tmp;
            } else
            {
                std::cout << "Error reallocating array";
            }
        }
        this->childs[this->child_count] = ptr;
        this->child_count++;
    }
    //dont forget to write a destructor))
};


#endif // PARSER_DATASTRUCTURES_H_INCLUDED
