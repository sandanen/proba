//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include <ScktComp.hpp>
#include "sButton.hpp"
#include "sEdit.hpp"
#include "sMemo.hpp"
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
        TLabel *LabelPort;
        TEdit *EditPort;
        TLabel *LabelHost;
        TEdit *EditHost;
        TButton *ButtonConnect;
        TMemo *MemoMessages;
        TEdit *EditMessage;
        TButton *ButtonSend;
        TClientSocket *ClientSocket;
        TsButton *ButtonStart;
        TServerSocket *ServerSocket;
        TsButton *ButtonSend2;
        TsButton *SaveButton;
        TsButton *ReadButton;
        void __fastcall ButtonConnectClick(TObject *Sender);
        void __fastcall ButtonSendClick(TObject *Sender);
        void __fastcall ClientSocketRead(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ButtonStartClick(TObject *Sender);
        void __fastcall ServerSocketClientRead(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ButtonSend2Click(TObject *Sender);
        void __fastcall SaveButtonClick(TObject *Sender);
        void __fastcall ReadButtonClick(TObject *Sender);
        


private:
void SendingMessages(AnsiString Message);// User declarations
public:		// User declarations
        __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
