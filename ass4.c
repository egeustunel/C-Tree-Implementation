#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXCHAR 1000

struct node {
    char letter;
    char* password;
    int child_num;
    struct node* parent;
    struct node* children[26];
};

struct node* create_node(struct node *parent, char letter, char *password){
    int i = 0;
    struct node* Node = malloc(sizeof(struct node));
    Node->letter = letter;
    Node->parent = parent;
    Node->password = password;
    Node->child_num = 0;
    for(i=0;i<26;i++)
    {
        Node->children[i] = NULL;
    }
    return Node;
}

struct node* add_child_node(struct node *parent, struct node *new_child){
    int i;
    //find first null child index in array and add
    for(i=0;i<26;i++)
    {
        if(parent->children[i]==NULL)
        {
            parent->children[i]=new_child;
            parent->child_num++;
            break;
        }
    }
}

char *search(char* name, struct node *root){

    int i,j;
    char* pass = " ";
    struct node *search_node=root;
    char letter;

    for(i=0;i<strlen(name);i++)
    {
        letter=name[i];
        for(j=0;j<26;j++){
            if((search_node->children[j] == NULL)){
                if(search_node==root){
                    return "no record";
                }
                else{
                    return "incorrect username"; //incorrect username
                }
            }

            else if(search_node->children[j]->letter==letter)
            {
                if(search_node->children[j]->password != " "){
                    pass=search_node->children[j]->password;
                }


                search_node=search_node->children[j]; // go child with next letter
                break; // continue to searching next letter of user name
            }

        }

    }
    if(pass != " "){
        return pass;
    }

    else
        return "not enough username"; //not enough user name


}

void find_last_node_exist(struct node *root, char* name, char* password){
    int i,j,k;
    struct node *newNode;

    for(i=0; i < strlen(name); i++){
            for(j = 0; j < root->child_num; j++){
                if(root->children[j]->letter == name[i]){
                    newNode = root->children[j];
                    char* name_chopped = name + 1;
                    find_last_node_exist(newNode, name_chopped, password);
                    return;
                }
            }
        }
        for(k=0;k<strlen(name);k++){
            if(k==strlen(name)-1){
                newNode = create_node(root, name[k], password);
            }
            else{
                newNode = create_node(root, name[k], " ");
            }
            add_child_node(root, newNode);
            root=newNode;
        }

}

void add(char* name, char* password, struct node *root, FILE* out){
    int i;
    char* search_result;
    struct node *parentNode=root;
    struct node *newNode;
    search_result = search(name, root);

    if(search_result == password){

        fprintf(out,"\"%s\" reserved username\n", name);

    }else if(search_result == "no record"){

        for(i=0;i<strlen(name);i++){
            if(i==strlen(name)-1){
                newNode = create_node(parentNode, name[i], password);
            }

            else{
                newNode = create_node(parentNode, name[i], " ");
            }
            add_child_node(parentNode, newNode);
            parentNode=newNode;
        }
        fprintf(out,"\"%s\" was added\n", name);
    }else if(search_result == "incorrect username"){

        find_last_node_exist(root, name, password);
        fprintf(out,"\"%s\" was added\n", name);


    }else if(search_result == "not enough username"){
        find_last_node_exist(root, name, password);
        fprintf(out,"\"%s\" was added\n", name);
    }
}

void del(char* name, struct node *root, FILE* out){
    fprintf(out,"delete\n");
}

char* get_inverse(struct node *lastNode, int *sublen){
    char name_reverse[50];
    int index=0;
    int j;
    struct node *n =lastNode;

    while(n->parent!=NULL)
    {
        name_reverse[index++]=n->letter;
        n=n->parent;
    }
    char *username;
    username=malloc(index);
    *sublen=index;
    for(j=index-1;j>=0;j--)
    {
        username[index-1-j]=name_reverse[j];
    }

    return username;
}

int list(struct node *root, FILE* out){
    int i,j, sublen=0;

    if(root->child_num==0){
        return;
    }

    for(i=0;i<root->child_num;i++){
        if(root->children[i]->child_num >= 2){

            char *username=get_inverse(root->children[i],&sublen);
            fprintf(out,"-");
            for(j=0;j<sublen;j++)
                fprintf(out,"%c",username[j]);
            fprintf(out,"\n");

        }
        if(root->children[i]->password != " "){
            char *username=get_inverse(root->children[i],&sublen);
            fprintf(out,"    -");
            for(j=0;j<sublen;j++){
                fprintf(out,"%c",username[j]);
            }
            fprintf(out,"\n");


        }
        list(root->children[i], out);
    }

}

int read_line(struct node *root){
    FILE *fp, *out;
    out = fopen("output.txt", "wb");
    char str[MAXCHAR];
    int i = 0;
    char* search_result;
    char* result;
    fp = fopen("input.txt", "r");
    if (fp == NULL){
        printf("Could not open file input.txt");
        return 1;
    }

    char *line[3];
    int count = 0;
    while (fgets(str, MAXCHAR, fp) != NULL){
        count = 0;
        char *token;

        /* first token */
        token = strtok(str, " \n");
        /* other tokens */

        while( token != NULL ) {


            token = strtok(NULL, " \n");

             line[count++] = token;
        }
        if(str[1] == 'a'){/*add function*/
            for(i = 0; line[0][i]; i++){
                line[0][i] = tolower(line[0][i]);
            }
            add(line[0], line[1], root, out);

        }if(str[1] == 's'){/*search function*/
            for(i = 0; line[0][i]; i++){
                line[0][i] = tolower(line[0][i]);
            }
            search_result = search(line[0], root);

        if(search_result == "no record"){

            fprintf(out,"\"%s\" no record\n", line[0]);

        }else if(search_result == "incorrect username"){

            fprintf(out,"\"%s\" incorrect username\n", line[0]);

        }else if(search_result == "not enough username"){

            fprintf(out,"\"%s\" not enough username\n", line[0]);

        }else{
            fprintf(out,"\"%s\" password %s\n", line[0], search_result);
        }
        }if(str[1] == 'q'){/*query function*/
            for(i = 0; line[0][i]; i++){
                line[0][i] = tolower(line[0][i]);
            }

            result=search(line[0], root);
            if(result == "no record")

                fprintf(out,"\"%s\" no record\n",line[0]);

            else if(result == "incorrect username")

                fprintf(out,"\"%s\" incorrect username\n",line[0]);

            else if(result == "not enough username")

                fprintf(out,"\"%s\" not enough username\n",line[0]);

            else {

                if(result==line[1])

                    fprintf(out,"\"%s\" successful login\n",line[0]);

                else

                    fprintf(out,"\"%s\" incorrect password\n",line[0]);

            }


        }if(str[1] == 'd'){/*delete function*/
            for(i = 0; line[0][i]; i++){
                line[0][i] = tolower(line[0][i]);
            }
            del(line[0], root, out);
        }if(str[1] == 'l'){/*list function*/
            list(root, out);
        }

    }
    fclose(fp);
    fclose(out);
}

int main(){
    /*root node*/
    struct node* root=create_node(NULL,NULL," ");
    read_line(root);
    return 0;
}
