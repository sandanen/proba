//---------------------------------------------------------------------------

#include <vcl.h>

#include <cstring>
#include <fstream>
#pragma hdrstop
#include "RC5MY.hpp"
#include "main.h"
#define KEY_LEN 16
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "sButton"
#pragma link "sEdit"
#pragma link "sMemo"
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
        : TForm(Owner)
{
}

void TFormMain::SendingMessages(AnsiString Message) 
{
     AnsiString text = Message;



  



  MemoMessages->Lines->Add(text);
  for(int i = 0; i < ServerSocket->Socket->ActiveConnections; i++)
  {
    ServerSocket->Socket->Connections[i]->SendText(text);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ButtonConnectClick(TObject *Sender)
{
        EditPort->Enabled = false;
        EditHost->Enabled = false;

        ButtonConnect->Enabled = false;
        ClientSocket->Port = EditPort->Text.ToInt();
        ClientSocket->Host = EditHost->Text;
        ClientSocket->Active = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ButtonSendClick(TObject *Sender)
{

        ClientSocket->Socket->SendText( EditMessage->Text);
        EditMessage->Text="";
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ClientSocketRead(TObject *Sender,
      TCustomWinSocket *Socket)
{
        MemoMessages->Lines->Add(Socket->ReceiveText());
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
        ClientSocket->Active = false;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ButtonStartClick(TObject *Sender)
{
EditPort->Enabled = false;
ButtonStart->Enabled = false;
ServerSocket->Port = EditPort->Text.ToInt();
ServerSocket->Active = true;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ServerSocketClientRead(TObject *Sender,
      TCustomWinSocket *Socket)
{
SendingMessages(Socket->ReceiveText());
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ButtonSend2Click(TObject *Sender)
{




   SendingMessages(EditMessage->Text);

   EditMessage->Text="";

}
//---------------------------------------------------------------------------


void __fastcall TFormMain::SaveButtonClick(TObject *Sender)    //запись в файл шиф
{

                AnsiString text = MemoMessages->Text;

             ofstream f;
             f.open("vih.txt");

             f<<text.c_str();

             f.close();
                              #define KEY_LEN 16
                                RC5Simple rc5;

unsigned char key[KEY_LEN];
for(int i=0; i<KEY_LEN; i++)
 key[i]=i*2;
vector<unsigned char> v_key(KEY_LEN);
for(int i=0; i<KEY_LEN; i++)
 v_key[i]=key[i];

   rc5.RC5_SetKey(v_key);

     rc5.RC5_EncryptFile("vih.txt", "zash.txt");
     remove("vih.txt") ;

}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ReadButtonClick(TObject *Sender)
{
          #define KEY_LEN 16
                                RC5Simple rc5;

unsigned char key[KEY_LEN];
for(int i=0; i<KEY_LEN; i++)
 key[i]=i*2;
vector<unsigned char> v_key(KEY_LEN);
for(int i=0; i<KEY_LEN; i++)
 v_key[i]=key[i];

   rc5.RC5_SetKey(v_key);
    rc5.RC5_DecryptFile("zash.txt", "vih.txt");



         ifstream f;                                   //загрузка зашифрован сообщ




              f.open("vih.txt");

           char s[30];
           while (!f.eof()){
              f.getline(s, 30);

             AnsiString a(s);
            MemoMessages->Lines->Add(a);
                  }

                 remove("vih.txt") ;






}
//---------------------------------------------------------------------------








