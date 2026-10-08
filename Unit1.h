//---------------------------------------------------------------------------
#ifndef ProyectoCriptoH
#define ProyectoCriptoH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <string>
#include "core\Tipos.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// Componentes administrados por el IDE (solo el form es visual)
	void __fastcall FormCreate(TObject *Sender);

private:	// ---- Controles (creados en ConstruirUI) ----
	TPageControl *pc;
	TStatusBar   *sb;

	// Pestana 1: Cifrar
	TRadioGroup  *rg1;
	TStringGrid  *grid1;
	TLabel       *lblDet1;
	TMemo        *memPlano1, *memCifrado1, *memDesglose1;

	// Pestana 2: Descifrar
	TRadioGroup  *rg2;
	TStringGrid  *grid2;
	TLabel       *lblDet2;
	TMemo        *memInv2, *memCripto2, *memPlano2, *memDesglose2;

	// Pestana 3: Criptoanalisis
	TRadioGroup  *rg3;
	TStringGrid  *gridP, *gridC;
	TEdit        *edtPlano3, *edtCripto3;
	TMemo        *memK3;
	TLabel       *lblComp3;

	// Pestana 4: Inversa
	TRadioGroup  *rg4;
	TStringGrid  *grid4;
	TLabel       *lblDet4, *lblVerif4;
	TMemo        *memPaso4, *memInv4, *memVerif4;

	// ---- Construccion de UI ----
	void ConstruirUI();
	TPanel*      NuevaPagina(const String& titulo);
	TPanel*      Tarjeta(TWinControl* padre, int x, int y, int w, int h);
	TLabel*      Etiqueta(TWinControl* padre, const String& txt, int x, int y,
	                      int tam, TColor col, bool negrita);
	TMemo*       NuevoMemo(TWinControl* padre, int x, int y, int w, int h,
	                       bool soloLectura, int tam = 11);
	TEdit*       NuevoEdit(TWinControl* padre, int x, int y, int w);
	TRadioGroup* NuevoRadio(TWinControl* padre, int x, int y, int w);
	TStringGrid* NuevaGrilla(TWinControl* padre, int x, int y);
	TPanel*      NuevoBoton(TWinControl* padre, const String& txt, int x, int y,
	                        int w, int h, TNotifyEvent ev, bool acento);
	void Redondear(TWinControl* c, int radio);

	// ---- Auxiliares de GUI (sin logica matematica) ----
	Mat    LeerMatriz(TStringGrid* g);
	void   EscribirMatriz(TStringGrid* g, const Mat& M);
	void   DimensionarGrilla(TStringGrid* g, int n);
	void   CargarClaveDefecto(TStringGrid* g, int n);
	void   LimpiarGrilla(TStringGrid* g);
	int    TamanoSel(TRadioGroup* rg);
	void   AplicarTamano(TRadioGroup* rg);
	void   RefrescarClave(TStringGrid* g);
	void   SetEstado(const String& msg, bool error = false);
	void   Fallo(const String& msg);
	String PadNum(int v, int ancho);
	String MatrizATexto(const Mat& M);
	String AumentadaATexto(const Mat& A, const Mat& B);
	String VectorATextoStr(const Vec& v);
	String Agrupar(const String& s, int n);
	String Desglose(const std::string& texto, const Mat& M, bool cifrar);
	Mat    BloquesAMatriz(const std::string& texto, int n);
	std::string ToStd(const String& s);
	String      ToStr(const std::string& s);

	// ---- Eventos ----
	void __fastcall RgClick(TObject *Sender);
	void __fastcall GridDrawCell(TObject *Sender, System::LongInt ACol, System::LongInt ARow,
                             const TRect &Rect, TGridDrawState State);
void __fastcall GridSetEditText(TObject *Sender, System::LongInt ACol, System::LongInt ARow,
                                const UnicodeString Value);
	void __fastcall BtnEnter(TObject *Sender);
	void __fastcall BtnLeave(TObject *Sender);
	void __fastcall PcDrawTab(TCustomTabControl *Control, int TabIndex,
	                          const TRect &Rect, bool Active);
	void __fastcall PcChange(TObject *Sender);

	void __fastcall BtnCargar1Click(TObject *Sender);
	void __fastcall BtnCifrarClick(TObject *Sender);
	void __fastcall BtnCopiarClick(TObject *Sender);

	void __fastcall BtnCargar2Click(TObject *Sender);
	void __fastcall BtnDescifrarClick(TObject *Sender);

	void __fastcall BtnLlenar3Click(TObject *Sender);
	void __fastcall BtnCargar3Click(TObject *Sender);
	void __fastcall BtnRecuperarClick(TObject *Sender);

	void __fastcall BtnCalcInvClick(TObject *Sender);
	void __fastcall BtnVerificarClick(TObject *Sender);

public:
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
