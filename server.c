#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <fcntl.h>
#include "DataStructures.c"
#define PORT 55769


struct communication{
    char message[1024];
    int flag; // 0 Read, 1 ReadWrite and 2 Close
};

void close_client(int client_desc){
    struct communication msg;
    strcpy(msg.message, "Logging Out, Good Bye\n");
    msg.flag = 2;
    write(client_desc, &msg, sizeof(msg));    
}
void Add_account(int client_desc){
    int fd  = open("customer_db", O_RDONLY, 0644);
    struct communication msg;
    char username[1024];
    char password[1024];
    char joint;
    strcpy(msg.message, "Enter Username: ");
    msg.flag = 1;
    write(client_desc, &msg, sizeof(msg));
    read(client_desc, username, sizeof(username));
    strcpy(msg.message, "Enter Password: ");
    msg.flag = 1;
    write(client_desc, &msg, sizeof(msg));
    read(client_desc, password, sizeof(password));
    struct Customer record;
    bool flag = 0;
    while(read(fd, &record, sizeof(record))){
        if(strcmp(record.username, username) == 0 && strcmp(record.password, password) == 0 && record.status){
            flag = 1;
            break;
        }
    }
    close(fd);
    if(flag){
        strcpy(msg.message, "username already exist\n");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        close_client(client_desc);
        return;
    }
    else{
    int fd  = open("customer_db", O_WRONLY|O_APPEND, 0644);
    struct Customer record;
    strcpy(record.username , username);
    strcpy(record.password, password);
    strncpy(record.account_num ,record.username,5);
    strcpy(record.account_num + 5, "12345");
    record.status = 1;
    record.type = 0;
    write(fd,&record,sizeof(record));
    close(fd);
    int fd1 = open("account_db", O_WRONLY|O_APPEND, 0644);
    struct Account new_account;
    strcpy(new_account.account_num , record.account_num);
    new_account.balance = 0.00;
    new_account.status = 1;
    write(fd1,&new_account,sizeof(new_account));
    strcpy(msg.message, "\nWelcome, ");
    strcat(msg.message, record.username);
    strcat(msg.message, "  your account Number is  ");
    strcat(msg.message, record.account_num);
    strcat(msg.message, "  and balance is 0.00\n\n");
    strcat(msg.message, "\n\n");
    msg.flag = 0;
    write(client_desc, &msg, sizeof(msg));
    close(fd1);
    admin_operations(client_desc);
    }


}
void Modify(int client_desc) {
    int fd  = open("customer_db", O_RDWR, 0644);
    struct communication msg;
    char username[1024];
    char new_password[1024];
    strcpy(msg.message, "Enter Username: ");
    msg.flag = 1;
    write(client_desc, &msg, sizeof(msg));
    read(client_desc, username, sizeof(username));
    struct Customer record;
    bool flag = 0;
    while(read(fd, &record, sizeof(record))){
        if(strcmp(record.username, username) == 0 && record.status){
            flag = 1;
            strcpy(msg.message, "Enter new_password: ");
            msg.flag = 1;
            write(client_desc, &msg, sizeof(msg));
            read(client_desc, new_password, sizeof(new_password));
            strcpy(record.password,new_password);
            lseek(fd,-sizeof(record),SEEK_CUR);
            write(fd,&record,sizeof(record));
            break;
        }
    }
    close(fd);
    if(flag==0){
        strcpy(msg.message, "username does not exist\n");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        close_client(client_desc);
        return;
    }
    else{
        strcpy(msg.message, "password changed successfully \n");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        admin_operations(client_desc);
    }

}
void Search(int client_desc){
    int fd  = open("customer_db", O_RDONLY, 0644);
    struct communication msg;
    char username[1024];
    strcpy(msg.message, "Enter Username: ");
    msg.flag = 1;
    write(client_desc, &msg, sizeof(msg));
    read(client_desc, username, sizeof(username));
    struct Customer record;
    bool flag = 0;
    while(read(fd, &record, sizeof(record))){
        if(strcmp(record.username, username) == 0  && record.status){
            flag = 1;
            break;
        }
    }
    close(fd);
    if(flag == 0 ){
        strcpy(msg.message, "user does not exist\n");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        close_client(client_desc);
        return;}
    else{
        int fd1  = open("account_db", O_RDONLY, 0644);
        struct Account a;
        while(read(fd1, &a, sizeof(a))){
        if(strcmp(a.account_num, record.account_num) == 0 && a.status){
            break;
        }
        }
        strcpy(msg.message, "\nHere account detials of , ");
        strcat(msg.message, record.username);
        strcat(msg.message, "   account Number is  ");
        strcat(msg.message, record.account_num);
        strcat(msg.message, "  and balance is ");
        msg.flag = 0;
        char amount1[20];
        gcvt(a.balance,6,amount1);
        strcat(msg.message, amount1);
        strcat(msg.message, "\n\n");
        write(client_desc, &msg, sizeof(msg));
        //ftoa(a.balance,)
        //strcpy(msg.message,  a.balance);
        //strcat(msg.message, "\n\n");
        //msg.flag = 0;
        //write(client_desc, &msg, sizeof(msg));
        close(fd1);
        admin_operations(client_desc);
    
        }
        

}
void Delete_Account(int client_desc){
    int fd  = open("customer_db", O_RDWR, 0644);
    struct communication msg;
    char username[1024];
    char account_number[1024];
    strcpy(msg.message, "Enter Username: ");
    msg.flag = 1;
    write(client_desc, &msg, sizeof(msg));
    read(client_desc, username, sizeof(username));
    struct Customer record;
    bool flag = 0;
    while(read(fd, &record, sizeof(record))){
        if(strcmp(record.username, username) == 0  && record.status){
            flag = 1;
            record.status = 0;
            lseek(fd,-sizeof(record),SEEK_CUR);
            write(fd,&record,sizeof(record));
            strcpy(account_number,record.account_num);
            break;
        }
    }
    close(fd);
    int fd1 = open("account_db", O_RDWR, 0644);
            struct Account del_Account;
            while(read(fd1, &del_Account, sizeof(del_Account))){
               if(strcmp(del_Account.account_num, record.account_num) == 0 &&  del_Account.status){
               
               del_Account.status = 0;
               lseek(fd1,-sizeof(del_Account),SEEK_CUR);
               write(fd1,&del_Account,sizeof(del_Account));
               break;
               }
               }
               close(fd1);
    if(flag==0){
        strcpy(msg.message, "user does not exist\n");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        close_client(client_desc);
        return;
    }
    else{
        strcpy(msg.message, "Account_deleted successfully\n");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        admin_operations(client_desc);
    }

}

void initial_setup(){
    int fd = open("customer_db", O_CREAT|O_EXCL|O_WRONLY, 0644);
    if(fd != -1){
        struct Customer admin;
        strcpy(admin.username, "admin");
        strcpy(admin.password, "admin");
        strcpy(admin.account_num, "0");
        admin.status = 1;
        admin.type = 1;
        write(fd, &admin, sizeof(admin));
        struct Customer c1;
        strcpy(c1.username, "ayushi");
        strcpy(c1.password, "ayushi");
        strcpy(c1.account_num, "ayushi1234");
        c1.status = 1;
        c1.type = 0;
        write(fd, &c1, sizeof(c1));
        close(fd);
    }
    int fd1 = open("account_db", O_CREAT|O_EXCL|O_WRONLY, 0644);
    if(fd1 != -1){
        struct Account a;
        a.balance = 1200.00;
        strcpy(a.account_num, "ayushi1234");
        a.status = 1;
        write(fd1, &a, sizeof(a));
        close(fd1);
    }
    open("transaction_db", O_CREAT|O_EXCL, 0644);
}

void admin_operations(int client_desc){
    struct communication msg;
    
    strcpy(msg.message, "Press 1 to add an account\nPress 2 to delete an account\nPress 3 to modify an account\nPress 4 to search for an account\nPress any other key to exit\nEnter your choice: ");
    msg.flag = 1;
    write(client_desc, &msg, sizeof(msg));
    char input[1024];
    read(client_desc, input, sizeof(input));
    if(strcmp(input, "1")==0){
        printf("Add account\n");
        Add_account(client_desc);
    }else if(strcmp(input, "2")==0){
        printf("Delete account\n");
        Delete_Account(client_desc);
    }else if(strcmp(input, "3")==0){
        printf("Modify account\n");
        Modify(client_desc);
    }else if(strcmp(input, "4")==0){
        printf("Search account\n");
        Search(client_desc);
    }else{
        close_client(client_desc);
    }
}
void Depoist(int client_desc,char username[1024]){
int fd  = open("customer_db", O_RDONLY, 0644);
    struct communication msg;
    struct Customer record;
    bool flag = 0;
    int fd2  = open("transaction_db", O_WRONLY|O_APPEND, 0644);
    struct Transaction t;
    while(read(fd, &record, sizeof(record))){
        if(strcmp(record.username, username) == 0  && record.status){
            flag = 1;
            char amount[20];
            char *ptr;
            strcpy(msg.message, "Enter the amount for depoist\n");
            msg.flag = 1;
            write(client_desc, &msg, sizeof(msg));
            read(client_desc,amount,sizeof(amount));
            t.amount = strtod(amount,&ptr);
            break;
        }
    }
    close(fd);
    int fd1 = open("account_db", O_RDWR, 0644);
    struct Account new_Account;
    while(read(fd1, &new_Account, sizeof(new_Account))){
               if(strcmp(new_Account.account_num, record.account_num) == 0 &&  new_Account.status){
               
               new_Account.balance = new_Account.balance + t.amount;
               lseek(fd1,-sizeof(new_Account),SEEK_CUR);
               write(fd1,&new_Account,sizeof(new_Account));
               break;
               }
               }
    time_t t1;
    time(&t1);           
    strcpy(t.date ,ctime(&t1));
    strcpy(t.account_num,new_Account.account_num);
    t.type = 'C';
    t.balance = new_Account.balance;
    write(fd2,&t1,sizeof(t1));
    char amount1[20];
    gcvt(t.balance,6,amount1);
    strcpy(msg.message, "\nWelcome, ");
    strcat(msg.message, record.username);
    strcat(msg.message, "  your account Number is  ");
    strcat(msg.message, record.account_num);
    strcat(msg.message, "  and balance is ");
    strcat(msg.message, amount1);
    msg.flag = 0;
    write(client_desc, &msg, sizeof(msg));
    close(fd1);
    close(fd2);
    normal_operations(client_desc,username);

}
void Withdraw(int client_desc,char username[1024]){
int fd  = open("customer_db", O_RDONLY, 0644);
    struct communication msg;
    struct Customer record;
    bool flag = 0;
    int fd2  = open("transaction_db", O_WRONLY|O_APPEND, 0644);
    struct Transaction t;
    while(read(fd, &record, sizeof(record))){
        if(strcmp(record.username, username) == 0  && record.status){
            flag = 1;
            char amount[20];
            char *ptr;
            strcpy(msg.message, "Enter the amount for Withdraw\n");
            msg.flag = 1;
            write(client_desc, &msg, sizeof(msg));
            read(client_desc,amount,sizeof(amount));
            t.amount = strtod(amount,&ptr);
            break;
        }
    }
    close(fd);
    int fd1 = open("account_db", O_RDWR, 0644);
    struct Account new_Account;
    while(read(fd1, &new_Account, sizeof(new_Account))){
               if(strcmp(new_Account.account_num, record.account_num) == 0 &&  new_Account.status){
               
               new_Account.balance = new_Account.balance - t.amount;
               lseek(fd1,-sizeof(new_Account),SEEK_CUR);
               write(fd1,&new_Account,sizeof(new_Account));
               break;
               }
               }
    time_t t1;
    time(&t1);           
    strcpy(t.date ,ctime(&t1));
    strcpy(t.account_num,new_Account.account_num);
    t.type = 'D';
    t.balance = new_Account.balance;
    write(fd2,&t1,sizeof(t1));
    strcpy(msg.message, "\nWelcome, ");
    strcat(msg.message, record.username);
    strcat(msg.message, "  your account Number is  ");
    strcat(msg.message, record.account_num);
    strcat(msg.message, "  and balance is \n\n");
    char amount1[20];
    gcvt(t.balance,6,amount1);
    strcat(msg.message, amount1);
    strcat(msg.message, "\n\n");
    msg.flag = 0;
    write(client_desc, &msg, sizeof(msg));
    close(fd1);
    close(fd2);
    normal_operations(client_desc,username);

}
void Change_password(int client_desc,char username[1024]) {
    int fd  = open("customer_db", O_RDWR, 0644);
    struct communication msg;
    char new_password[1024];
    char old_password[1024];
    strcpy(msg.message, "Enter old_password\n");
    msg.flag = 1;
    write(client_desc, &msg, sizeof(msg));
    read(client_desc,&old_password,sizeof(old_password));
    struct Customer record;
    bool flag = 0;
    while(read(fd, &record, sizeof(record))){
        if(strcmp(record.username, username) == 0 && strcmp(record.password,old_password) == 0 && record.status){
            flag = 1;
            strcpy(msg.message, "Enter new_password: ");
            msg.flag = 1;
            write(client_desc, &msg, sizeof(msg));
            read(client_desc, new_password, sizeof(new_password));
            strcpy(record.password,new_password);
            lseek(fd,-sizeof(record),SEEK_CUR);
            write(fd,&record,sizeof(record));
            break;
        }
    }
    close(fd);
    if(flag==0){
        strcpy(msg.message, "Enter correct old_password\n");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        close_client(client_desc);
        return;
    }
    else{
        
        strcpy(msg.message, "password changed successfully \n");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        strcpy(msg.message, "login again \n");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        close_client(client_desc);
        return;
    }

}
void Account_details(int client_desc,char username[1024]){
    int fd  = open("customer_db", O_RDONLY, 0644);
    struct communication msg;
    struct Customer record;
    bool flag = 0;
    while(read(fd, &record, sizeof(record))){
        if(strcmp(record.username, username) == 0  && record.status){
            flag = 1;
            break;
        }
    }
    close(fd);
    if(flag == 0 ){
        strcpy(msg.message, "user does not exist\n");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        close_client(client_desc);
        return;}
    else{
        int fd1  = open("account_db", O_RDONLY, 0644);
        struct Account a;
        while(read(fd1, &a, sizeof(a))){
        if(strcmp(a.account_num, record.account_num) == 0 && a.status){
            break;
        }
        }
        strcpy(msg.message, "\nuserame : ");
        strcat(msg.message, record.username);
        strcat(msg.message, "   account Number is  ");
        strcat(msg.message, record.account_num);
        strcat(msg.message, "  and balance is ");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        //ftoa(a.balance,)
        //strcpy(msg.message,  a.balance);
        close(fd1);
        normal_operations(client_desc,username);
    
        }
        

}
void normal_operations(int client_desc,char username[1024]){
    struct communication msg;
    
    strcpy(msg.message, "Press 1 for depoist\nPress 2 to for withdrawal\nPress 3 to change the passsword\nPress 4 for account details  \nPress any other key to exit\nEnter your choice: ");
    msg.flag = 1;
    write(client_desc, &msg, sizeof(msg));
    char input[1024];
   int  r=read(client_desc, input, sizeof(input));
   printf("%s",input);
    if(strcmp(input , "1")==0){
        printf("depoist\n");
        Depoist(client_desc,username);
    }else if(strcmp(input , "2")==0){
        printf("Withdraw\n");
        Withdraw(client_desc,username);
    }else if(strcmp(input , "3")==0){
        printf("change password\n");
        Change_password(client_desc,username);
    }else if(strcmp(input , "4")==0){
        printf("account_details\n");
        Account_details(client_desc,username);
    }else{
         printf("%s",input);
         printf("%d",r);
        close_client(client_desc);
    }
}

void validating_credentials(int client_desc){
    struct communication msg;
    char username[1024];
    char password[1024];
    strcpy(msg.message, "Enter Username: ");
    msg.flag = 1;
    write(client_desc, &msg, sizeof(msg));
    read(client_desc, username, sizeof(username));
    strcpy(msg.message, "Enter Password: ");
    msg.flag = 1;
    write(client_desc, &msg, sizeof(msg));
    read(client_desc, password, sizeof(password));
    
    int fd = open("customer_db", O_RDONLY);
    struct Customer record;
    bool flag = 0;
    while(read(fd, &record, sizeof(record))){
        if(strcmp(record.username, username) == 0 && strcmp(record.password, password) == 0 && record.status){
            flag = 1;
            break;
        }
    }
    close(fd);
    if(!flag){
        strcpy(msg.message, "Invalid username/password\n");
        msg.flag = 0;
        write(client_desc, &msg, sizeof(msg));
        close_client(client_desc);
        return;
    }

    strcpy(msg.message, "\nWelcome, ");
    strcat(msg.message, record.username);
    strcat(msg.message, "\n\n");
    msg.flag = 0;
    write(client_desc, &msg, sizeof(msg));
    
    if(record.type){
        admin_operations(client_desc);
        // admin operations
    }else{
         normal_operations(client_desc,username);
        // normal operations
    }
    return;

}


int main(int argc, char const *argv[]){
    struct sockaddr_in server, client;
    int socket_desc, size_client, client_desc;
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    initial_setup();

    server.sin_port = htons(PORT);
    if(bind(socket_desc, (void *)(&server), sizeof(server)) < 0) {
        perror("Error on binding:");
        exit(EXIT_FAILURE);
    }
    listen(socket_desc, 5);
    while(1){
        size_client = sizeof(client);
        if((client_desc = accept(socket_desc, (struct sockaddr*)&client, &size_client)) < 0) {
            perror("Error on accept:");
            exit(EXIT_FAILURE);
        }
        if(fork() == 0){
            struct communication msg;
            strcpy(msg.message, "------------------------WELCOME TO ONLINE BANKING------------------------\n");
            msg.flag = 0;
            write(client_desc, &msg, sizeof(msg));
            validating_credentials(client_desc);
        }
    }
    return 0;
}
