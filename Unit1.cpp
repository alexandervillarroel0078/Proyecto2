//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <Vcl.Clipbrd.hpp>
#include <stdexcept>
#include "Unit1.h"
#include "core\AlfabetoHill.h"
#include "core\MatrizMod26.h"
#include "core\GaussJordan.h"
#include "core\HillCipher.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

// ---- Paleta ----
// ---- Paleta corporativa ----
static const TColor CL_BG      = static_cast<TColor>(RGB(0xF5, 0xEE, 0xE9)); // beige crema
static const TColor CL_CARD    = static_cast<TColor>(RGB(0xFF, 0xFF, 0xFF)); // blanco
static const TColor CL_ACCENT  = static_cast<TColor>(RGB(0x12, 0x71, 0x5B)); // verde principal
static const TColor CL_ACC_HOV = static_cast<TColor>(RGB(0x1A, 0x8F, 0x73)); // verde hover
static const TColor CL_ACC_DK  = static_cast<TColor>(RGB(0x0B, 0x55, 0x44)); // verde oscuro
static const TColor CL_SEC     = static_cast<TColor>(RGB(0xEA, 0xDF, 0xD6)); // crema oscuro
static const TColor CL_SEC_HOV = static_cast<TColor>(RGB(0xDF, 0xD1, 0xC4)); // crema hover
static const TColor CL_BORDER  = static_cast<TColor>(RGB(0xD9, 0xCB, 0xC0)); // borde
static const TColor CL_TEXT    = static_cast<TColor>(RGB(0x1F, 0x1A, 0x17)); // texto principal
static const TColor CL_TEXT2   = static_cast<TColor>(RGB(0x6B, 0x5E, 0x57)); // texto secundario
static const TColor CL_OK      = static_cast<TColor>(RGB(0x12, 0x71, 0x5B)); // verde
static const TColor CL_ERR     = static_cast<TColor>(RGB(0x71, 0x12, 0x28)); // vino
// ---- Claves sugeridas ----
static const int K3_DEF[3][3] = { {3, 10, 20}, {20, 9, 17}, {9, 4, 15} };
static const int K4_DEF[4][4] = { {8, 6, 9, 5}, {6, 9, 5, 10}, {5, 8, 4, 9}, {10, 6, 11, 4} };

#define S_INV  L"\u207B\u00B9"   // superindice -1
#define S_DOT  L"\u00B7"
#define S_ARR  L"\u2192"

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
	ConstruirUI();
}

// ===========================================================================
//  CONSTRUCCION DE LA INTERFAZ
// ===========================================================================
void TForm1::ConstruirUI()
{
	Caption = L"Hill Cipher Lab";
	Color = CL_BG;
	BorderStyle = bsSingle;
	BorderIcons = BorderIcons >> biMaximize;
	Position = poScreenCenter;
	ClientWidth = 1100;
	ClientHeight = 730;
	Font->Name = L"Segoe UI";
	Font->Size = 11;
	Font->Color = CL_TEXT;

	// Encabezado
	TPanel* hdr = new TPanel(this);
	hdr->Parent = this;
	hdr->Align = alTop;
	hdr->Height = 64;
	hdr->BevelOuter = bvNone;
	hdr->ParentBackground = false;
	hdr->Color = CL_BG;
	hdr->Caption = L"";
	Etiqueta(hdr, L"Hill Cipher Lab", 24, 8, 18, CL_TEXT, true);
	Etiqueta(hdr, L"Cifrado de Hill " S_DOT L" Criptoan\u00E1lisis por Gauss-Jordan " S_DOT
				  L" Alfabeto original de Hill (A=5, B=23, C=2, ...)",
			 26, 40, 9, CL_TEXT2, false);

	// Barra de estado
	sb = new TStatusBar(this);
	sb->Parent = this;
	sb->SimplePanel = true;
	sb->SimpleText = L"  Listo";

	// PageControl
	pc = new TPageControl(this);
	pc->Parent = this;
	pc->Align = alClient;
	pc->AlignWithMargins = true;
	pc->Margins->SetBounds(12, 0, 12, 4);
	pc->OwnerDraw = true;
	pc->TabHeight = 34;
	pc->TabWidth = 160;
	pc->HotTrack = true;
	pc->OnDrawTab = PcDrawTab;
	pc->OnChange = PcChange;

	// ---------------- Pestana 1: CIFRAR ----------------
	TPanel* p1 = NuevaPagina(L"Cifrar");
	TPanel* c1 = Tarjeta(p1, 16, 16, 420, 600);
	Etiqueta(c1, L"Matriz clave K", 20, 14, 12, CL_TEXT, true);
	rg1 = NuevoRadio(c1, 20, 44, 380);
	grid1 = NuevaGrilla(c1, 20, 104);
	lblDet1 = Etiqueta(c1, L"", 20, 262, 10, CL_TEXT2, false);
	NuevoBoton(c1, L"Cargar caso oficial 4x4", 20, 296, 380, 36, BtnCargar1Click, false);
	Etiqueta(c1, L"Texto plano", 20, 350, 10, CL_TEXT2, false);
	memPlano1 = NuevoMemo(c1, 20, 374, 380, 120, false);
	NuevoBoton(c1, L"Cifrar", 20, 512, 380, 44, BtnCifrarClick, true);

	TPanel* d1 = Tarjeta(p1, 452, 16, 608, 600);
	Etiqueta(d1, L"Texto cifrado", 20, 14, 12, CL_TEXT, true);
	memCifrado1 = NuevoMemo(d1, 20, 44, 480, 70, true);
	NuevoBoton(d1, L"Copiar", 512, 44, 76, 36, BtnCopiarClick, false);
	Etiqueta(d1, L"Desglose por bloque", 20, 132, 12, CL_TEXT, true);
	memDesglose1 = NuevoMemo(d1, 20, 162, 568, 420, true, 10);

	// ---------------- Pestana 2: DESCIFRAR ----------------
	TPanel* p2 = NuevaPagina(L"Descifrar");
	TPanel* c2 = Tarjeta(p2, 16, 16, 420, 600);
	Etiqueta(c2, L"Matriz clave K", 20, 14, 12, CL_TEXT, true);
	rg2 = NuevoRadio(c2, 20, 44, 380);
	grid2 = NuevaGrilla(c2, 20, 104);
	lblDet2 = Etiqueta(c2, L"", 20, 262, 10, CL_TEXT2, false);
	NuevoBoton(c2, L"Cargar caso oficial 4x4", 20, 296, 380, 36, BtnCargar2Click, false);
	Etiqueta(c2, L"Matriz inversa K" S_INV L" mod 26", 20, 350, 12, CL_TEXT, true);
	memInv2 = NuevoMemo(c2, 20, 380, 380, 110, true);
	TLabel* ley2 = Etiqueta(c2, L"Calculada por Gauss-Jordan sobre [K | I] " S_ARR L" [I | K" S_INV L"]",
							20, 500, 9, CL_TEXT2, false);
	ley2->WordWrap = true; ley2->AutoSize = false; ley2->Width = 380; ley2->Height = 36;

	TPanel* d2 = Tarjeta(p2, 452, 16, 608, 600);
	Etiqueta(d2, L"Criptograma", 20, 14, 12, CL_TEXT, true);
	memCripto2 = NuevoMemo(d2, 20, 44, 568, 70, false);
	NuevoBoton(d2, L"Descifrar", 20, 126, 568, 44, BtnDescifrarClick, true);
	Etiqueta(d2, L"Texto plano recuperado", 20, 186, 12, CL_TEXT, true);
	memPlano2 = NuevoMemo(d2, 20, 216, 568, 64, true);
	Etiqueta(d2, L"Desglose por bloque", 20, 296, 12, CL_TEXT, true);
	memDesglose2 = NuevoMemo(d2, 20, 326, 568, 256, true, 10);

	// ---------------- Pestana 3: CRIPTOANALISIS ----------------
	TPanel* p3 = NuevaPagina(L"Criptoan\u00E1lisis");
	TPanel* c3 = Tarjeta(p3, 16, 16, 520, 600);
	Etiqueta(c3, L"Datos conocidos (texto plano y cifrado)", 20, 14, 12, CL_TEXT, true);
	rg3 = NuevoRadio(c3, 20, 44, 480);
	Etiqueta(c3, L"Matriz P (columnas = bloques del texto plano)", 20, 104, 9, CL_TEXT2, false);
	Etiqueta(c3, L"Matriz C (columnas = bloques del cifrado)", 270, 104, 9, CL_TEXT2, false);
	gridP = NuevaGrilla(c3, 20, 128);
	gridC = NuevaGrilla(c3, 270, 128);
	Etiqueta(c3, L"Texto plano", 20, 292, 10, CL_TEXT2, false);
	edtPlano3 = NuevoEdit(c3, 20, 314, 230);
	Etiqueta(c3, L"Criptograma", 270, 292, 10, CL_TEXT2, false);
	edtCripto3 = NuevoEdit(c3, 270, 314, 230);
	NuevoBoton(c3, L"Llenar P y C desde el texto", 20, 358, 480, 34, BtnLlenar3Click, false);
	NuevoBoton(c3, L"Cargar caso oficial 4x4", 20, 402, 480, 34, BtnCargar3Click, false);
	Etiqueta(c3, L"K = C " S_DOT L" P" S_INV L" mod 26", 20, 452, 12, CL_ACC_HOV, true);
	NuevoBoton(c3, L"Recuperar clave K", 20, 486, 480, 44, BtnRecuperarClick, true);

	TPanel* d3 = Tarjeta(p3, 552, 16, 508, 600);
	Etiqueta(d3, L"Clave K recuperada", 20, 14, 12, CL_TEXT, true);
	memK3 = NuevoMemo(d3, 20, 44, 468, 170, true);
	lblComp3 = Etiqueta(d3, L"K recuperada == K original?  \u2014", 20, 232, 12, CL_TEXT2, true);
	TLabel* exp3 = Etiqueta(d3,
		L"Cada columna de P es un bloque del texto plano y cada columna de C su bloque cifrado. "
		L"Como C = K " S_DOT L" P, se obtiene K = C " S_DOT L" P" S_INV L" mod 26, con P" S_INV
		L" calculada por Gauss-Jordan. La comparaci\u00F3n se hace contra la clave K de la pesta\u00F1a Cifrar.",
		20, 280, 10, CL_TEXT2, false);
	exp3->WordWrap = true; exp3->AutoSize = false; exp3->Width = 468; exp3->Height = 120;

	// ---------------- Pestana 4: INVERSA ----------------
	TPanel* p4 = NuevaPagina(L"Inversa");
	TPanel* c4 = Tarjeta(p4, 16, 16, 420, 600);
	Etiqueta(c4, L"Matriz K", 20, 14, 12, CL_TEXT, true);
	rg4 = NuevoRadio(c4, 20, 44, 380);
	grid4 = NuevaGrilla(c4, 20, 104);
	lblDet4 = Etiqueta(c4, L"", 20, 262, 10, CL_TEXT2, false);
	NuevoBoton(c4, L"Calcular K" S_INV, 20, 296, 380, 44, BtnCalcInvClick, true);
	NuevoBoton(c4, L"Verificar K " S_DOT L" K" S_INV L" = I", 20, 350, 380, 36, BtnVerificarClick, false);
	Etiqueta(c4, L"Matriz K" S_INV L" final", 20, 402, 12, CL_TEXT, true);
	memInv4 = NuevoMemo(c4, 20, 432, 380, 150, true);

	TPanel* d4 = Tarjeta(p4, 452, 16, 608, 600);
	Etiqueta(d4, L"Paso a paso Gauss-Jordan sobre [K | I]", 20, 14, 12, CL_TEXT, true);
	memPaso4 = NuevoMemo(d4, 20, 44, 568, 290, true);
	Etiqueta(d4, L"Verificaci\u00F3n K " S_DOT L" K" S_INV, 20, 348, 12, CL_TEXT, true);
	memVerif4 = NuevoMemo(d4, 20, 378, 568, 150, true);
	lblVerif4 = Etiqueta(d4, L"", 20, 542, 12, CL_TEXT2, true);

	// Estado inicial: 4x4 en todas las pestanas (el caso oficial es 4x4)
	AplicarTamano(rg1);
	AplicarTamano(rg2);
	AplicarTamano(rg3);
	AplicarTamano(rg4);
	SetEstado(L"Listo");
}

TPanel* TForm1::NuevaPagina(const String& titulo)
{
	TTabSheet* ts = new TTabSheet(this);
	ts->PageControl = pc;
	ts->Caption = titulo;
	TPanel* p = new TPanel(ts);
	p->Parent = ts;
	p->Align = alClient;
	p->BevelOuter = bvNone;
	p->ParentBackground = false;
	p->Color = CL_BG;
	p->Caption = L"";
	return p;
}

TPanel* TForm1::Tarjeta(TWinControl* padre, int x, int y, int w, int h)
{
	TPanel* p = new TPanel(this);
	p->Parent = padre;
	p->SetBounds(x, y, w, h);
	p->BevelOuter = bvNone;
	p->ParentBackground = false;
	p->Color = CL_CARD;
	p->Caption = L"";
	Redondear(p, 18);
	return p;
}

TLabel* TForm1::Etiqueta(TWinControl* padre, const String& txt, int x, int y,
						 int tam, TColor col, bool negrita)
{
	TLabel* l = new TLabel(this);
	l->Parent = padre;
	l->Left = x;
	l->Top = y;
	l->Caption = txt;
	l->Transparent = true;
	l->Font->Size = tam;
	l->Font->Color = col;
	if (negrita) {
        l->Font->Name = L"Segoe UI Semibold";   // ← en vez de fsBold
    } else {
        l->Font->Name = L"Segoe UI";
    }
	return l;
}

TMemo* TForm1::NuevoMemo(TWinControl* padre, int x, int y, int w, int h,
						 bool soloLectura, int tam)
{
	TPanel* marco = new TPanel(this);
	marco->Parent = padre;
	marco->SetBounds(x, y, w, h);
	marco->BevelOuter = bvNone;
	marco->ParentBackground = false;
	marco->Color = CL_BG;
	marco->Caption = L"";
	Redondear(marco, 12);

	TMemo* m = new TMemo(this);
	m->Parent = marco;
	m->Align = alClient;
	m->AlignWithMargins = true;
	m->Margins->SetBounds(10, 8, 10, 8);
	m->BorderStyle = bsNone;
	m->Color = CL_BG;
	m->Font->Name = L"Cascadia Mono";
	m->Font->Size = 12;
	m->Font->Color = CL_TEXT;
	m->ReadOnly = soloLectura;
	m->WordWrap = false;
	m->ScrollBars = ssBoth;
	m->Text = L"";
	return m;
}

TEdit* TForm1::NuevoEdit(TWinControl* padre, int x, int y, int w)
{
	TEdit* e = new TEdit(this);
	e->Parent = padre;
	e->SetBounds(x, y, w, 30);
	e->Color = CL_BG;
	e->Font->Name = L"Consolas";
	e->Font->Size = 11;
	e->Font->Color = CL_TEXT;
	e->Text = L"";
	return e;
}

TRadioGroup* TForm1::NuevoRadio(TWinControl* padre, int x, int y, int w)
{
	TRadioGroup* r = new TRadioGroup(this);
	r->Parent = padre;
	r->SetBounds(x, y, w, 48);
	r->Caption = L"Tama\u00F1o de la matriz";
	r->Columns = 2;
	r->Items->Add(L"3x3");
	r->Items->Add(L"4x4");
	r->ParentBackground = false;
	r->Color = CL_CARD;
	r->Font->Color = CL_TEXT2;
	r->ItemIndex = 1;          // antes de asignar OnClick
	r->OnClick = RgClick;
	return r;
}

TStringGrid* TForm1::NuevaGrilla(TWinControl* padre, int x, int y)
{
	TStringGrid* g = new TStringGrid(this);
	g->Parent = padre;
	g->FixedCols = 0;
	g->FixedRows = 0;
	g->ColCount = 4;
	g->RowCount = 4;
	g->DefaultColWidth = 56;
	g->DefaultRowHeight = 36;
	g->GridLineWidth = 0;
	g->ScrollBars = ssNone;
	g->BorderStyle = bsNone;
	g->Color = CL_CARD;
	g->Font->Name = L"Cascadia Mono";
	g->Font->Size = 12;
	g->Font->Color = CL_TEXT;
	g->Options = TGridOptions() << goEditing << goTabs;
	g->DefaultDrawing = false;
	g->Left = x;
	g->Top = y;
	g->Width = 4 * 56;
	g->Height = 4 * 36;
	g->OnDrawCell = GridDrawCell;
	g->OnSetEditText = GridSetEditText;
	return g;
}

TPanel* TForm1::NuevoBoton(TWinControl* padre, const String& txt, int x, int y,
						   int w, int h, TNotifyEvent ev, bool acento)
{
	TPanel* b = new TPanel(this);
	b->Parent = padre;
	b->SetBounds(x, y, w, h);
	b->Caption = txt;
	b->BevelOuter = bvNone;
	b->ParentBackground = false;
	b->Color = acento ? CL_ACCENT : CL_SEC;
	b->Tag = acento ? 0 : 1;
	//b->Font->Color = clWhite; cambio de tipo letra
	b->Font->Color = acento ? clWhite : CL_TEXT;
	b->Font->Style = TFontStyles() << fsBold;
	b->Cursor = crHandPoint;
	b->OnClick = ev;
	b->OnMouseEnter = BtnEnter;
	b->OnMouseLeave = BtnLeave;
	Redondear(b, 12);
	return b;
}

void TForm1::Redondear(TWinControl* c, int radio)
{
	SetWindowRgn(c->Handle,
				 CreateRoundRectRgn(0, 0, c->Width + 1, c->Height + 1, radio, radio),
				 TRUE);
}

// ===========================================================================
//  AUXILIARES DE GUI
// ===========================================================================
std::string TForm1::ToStd(const String& s)
{
	AnsiString a(s);
	return std::string(a.c_str());
}

String TForm1::ToStr(const std::string& s)
{
	return String(s.c_str());
}

Mat TForm1::LeerMatriz(TStringGrid* g)
{
	int filas = g->RowCount, cols = g->ColCount;
	Mat M(filas, Vec(cols, 0));
	for (int i = 0; i < filas; i++) {
		for (int j = 0; j < cols; j++) {
			int v;
			if (!TryStrToInt(g->Cells[j][i].Trim(), v))
				throw Exception(String(L"Valor no num\u00E9rico en fila ") + IntToStr(i + 1) +
								L", columna " + IntToStr(j + 1));
			M[i][j] = MatrizMod26::mod(v);
		}
	}
	return M;
}

void TForm1::EscribirMatriz(TStringGrid* g, const Mat& M)
{
	for (size_t i = 0; i < M.size(); i++)
		for (size_t j = 0; j < M[i].size(); j++)
			g->Cells[(int)j][(int)i] = IntToStr(M[i][j]);
}

void TForm1::DimensionarGrilla(TStringGrid* g, int n)
{
	g->ColCount = n;
	g->RowCount = n;
	g->Width = n * g->DefaultColWidth;
	g->Height = n * g->DefaultRowHeight;
}

void TForm1::CargarClaveDefecto(TStringGrid* g, int n)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			g->Cells[j][i] = IntToStr(n == 3 ? K3_DEF[i][j] : K4_DEF[i][j]);
}

void TForm1::LimpiarGrilla(TStringGrid* g)
{
	for (int i = 0; i < g->RowCount; i++)
		for (int j = 0; j < g->ColCount; j++)
			g->Cells[j][i] = L"0";
}

int TForm1::TamanoSel(TRadioGroup* rg)
{
	return rg->ItemIndex == 0 ? 3 : 4;
}

void TForm1::AplicarTamano(TRadioGroup* rg)
{
	int n = TamanoSel(rg);
	if (rg == rg3) {
		DimensionarGrilla(gridP, n); LimpiarGrilla(gridP);
		DimensionarGrilla(gridC, n); LimpiarGrilla(gridC);
		memK3->Clear();
		lblComp3->Caption = L"K recuperada == K original?  \u2014";
		lblComp3->Font->Color = CL_TEXT2;
		return;
	}
	TStringGrid* g = (rg == rg1) ? grid1 : (rg == rg2) ? grid2 : grid4;
	DimensionarGrilla(g, n);
	CargarClaveDefecto(g, n);
	RefrescarClave(g);
}

void TForm1::RefrescarClave(TStringGrid* g)
{
	TLabel* l = (g == grid1) ? lblDet1 : (g == grid2) ? lblDet2 : lblDet4;
	try {
		Mat K = LeerMatriz(g);
		int d = MatrizMod26::mod(MatrizMod26::determinante(K));
		bool ok = MatrizMod26::esInvertible(K);
		l->Caption = String(L"det K mod 26 = ") + IntToStr(d) + L"  " S_DOT L"  " +
					 (ok ? L"invertible" : L"no invertible");
		l->Font->Color = ok ? CL_OK : CL_ERR;
		if (g == grid2) {
			if (ok) memInv2->Text = MatrizATexto(GaussJordan::invertir(K));
			else    memInv2->Text = L"K no es invertible mod 26";
		}
	}
	catch (...) {   // valor a medio escribir: no molestar al usuario
		l->Caption = L"det K mod 26 = \u2014";
		l->Font->Color = CL_TEXT2;
		if (g == grid2) memInv2->Text = L"";
	}
}

void TForm1::SetEstado(const String& msg, bool error)
{
	sb->SimpleText = L"  " + msg;
	sb->Repaint();
}

void TForm1::Fallo(const String& msg)
{
	SetEstado(L"Error: " + msg, true);
	ShowMessage(msg);
}

String TForm1::PadNum(int v, int ancho)
{
	String s = IntToStr(v);
	while (s.Length() < ancho) s = L" " + s;
	return s;
}

String TForm1::MatrizATexto(const Mat& M)
{
	String s;
	for (size_t i = 0; i < M.size(); i++) {
		s += L"[ ";
		for (size_t j = 0; j < M[i].size(); j++) s += PadNum(M[i][j], 3) + L" ";
		s += L"]";
		if (i + 1 < M.size()) s += sLineBreak;
	}
	return s;
}

String TForm1::AumentadaATexto(const Mat& A, const Mat& B)
{
	String s;
	for (size_t i = 0; i < A.size(); i++) {
		s += L"[ ";
		for (size_t j = 0; j < A[i].size(); j++) s += PadNum(A[i][j], 3) + L" ";
		s += L"| ";
		for (size_t j = 0; j < B[i].size(); j++) s += PadNum(B[i][j], 3) + L" ";
		s += L"]";
		if (i + 1 < A.size()) s += sLineBreak;
	}
	return s;
}

String TForm1::VectorATextoStr(const Vec& v)
{
	String s = L"[";
	for (size_t i = 0; i < v.size(); i++) {
		if (i) s += L" ";
		s += IntToStr(v[i]);
	}
	return s + L"]";
}

String TForm1::Agrupar(const String& s, int n)
{
	String r;
	for (int i = 1; i <= s.Length(); i++) {
		r += s[i];
		if (i % n == 0 && i < s.Length()) r += L" ";
	}
	return r;
}

// Desglose: M = K (cifrar) o K^-1 (descifrar). Todo el calculo lo hace el nucleo.
String TForm1::Desglose(const std::string& texto, const Mat& M, bool cifrar)
{
	int n = (int)M.size();
	std::string t = AlfabetoHill::rellenar(AlfabetoHill::limpiar(texto), n);
	std::vector<Vec> bloques = HillCipher::dividirBloques(HillCipher::textoAVector(t), n);

	String ent  = cifrar ? L"P" : L"C";
	String sal  = cifrar ? L"C" : L"P";
	String form = cifrar ? String(L"K" S_DOT L"P mod 26") : String(L"K" S_INV S_DOT L"C mod 26");

	String s;
	for (size_t b = 0; b < bloques.size(); b++) {
		Vec out = MatrizMod26::multiplicar(M, bloques[b]);
		s += L"Bloque " + IntToStr((int)b + 1) + L" " S_DOT L" " + ent + L" = " +
			 VectorATextoStr(bloques[b]) + L" " S_ARR L" " + sal + L" = " + form +
			 L" " S_ARR L" " + VectorATextoStr(out) + L" " S_ARR L" " +
			 ToStr(HillCipher::vectorATexto(out)) + sLineBreak;
	}
	return s;
}

// Columnas de la matriz = bloques del texto (n bloques de n letras)
Mat TForm1::BloquesAMatriz(const std::string& texto, int n)
{
	std::string t = AlfabetoHill::limpiar(texto);
	if ((int)t.size() != n * n)
		throw Exception(String(L"El texto debe tener exactamente ") + IntToStr(n * n) +
						L" letras (" + IntToStr(n) + L" bloques de " + IntToStr(n) + L")");
	std::vector<Vec> bl = HillCipher::dividirBloques(HillCipher::textoAVector(t), n);
	Mat M(n, Vec(n, 0));
	for (int b = 0; b < n; b++)
		for (int i = 0; i < n; i++)
			M[i][b] = bl[b][i];
	return M;
}

// ===========================================================================
//  EVENTOS GENERICOS
// ===========================================================================
void __fastcall TForm1::RgClick(TObject *Sender)
{
	AplicarTamano(static_cast<TRadioGroup*>(Sender));
	SetEstado(L"Listo");
}

//Cambios en la grilla: refrescar determinante y matriz inversa (si aplica)
void __fastcall TForm1::GridSetEditText(TObject *Sender, System::LongInt ACol, System::LongInt ARow,
                                        const UnicodeString Value)
{
    if (Sender == grid1 || Sender == grid2 || Sender == grid4)
        RefrescarClave(static_cast<TStringGrid*>(Sender));
}

void __fastcall TForm1::GridDrawCell(TObject *Sender, System::LongInt ACol, System::LongInt ARow,
                                     const TRect &Rect, TGridDrawState State)
{
    TStringGrid* g = static_cast<TStringGrid*>(Sender);
    TCanvas* c = g->Canvas;
    bool sel = State.Contains(gdSelected) || State.Contains(gdFocused);

    c->Brush->Style = bsSolid;
    c->Brush->Color = CL_CARD;
    c->FillRect(Rect);
    c->Brush->Color = sel ? CL_ACC_DK : CL_BG;
    c->Pen->Color = sel ? CL_ACCENT : CL_BORDER;
    c->RoundRect(Rect.Left + 2, Rect.Top + 2, Rect.Right - 2, Rect.Bottom - 2, 10, 10);

    String s = g->Cells[(int)ACol][(int)ARow];
    c->Font->Assign(g->Font);
    c->Font->Color = CL_TEXT;
    c->Brush->Style = bsClear;
    int tw = c->TextWidth(s), th = c->TextHeight(s);
    c->TextOut(Rect.Left + (Rect.Width() - tw) / 2, Rect.Top + (Rect.Height() - th) / 2, s);
}
void __fastcall TForm1::BtnEnter(TObject *Sender)
{
	TPanel* b = static_cast<TPanel*>(Sender);
	b->Color = (b->Tag == 0) ? CL_ACC_HOV : CL_SEC_HOV;
}

void __fastcall TForm1::BtnLeave(TObject *Sender)
{
	TPanel* b = static_cast<TPanel*>(Sender);
	b->Color = (b->Tag == 0) ? CL_ACCENT : CL_SEC;
}

void __fastcall TForm1::PcDrawTab(TCustomTabControl *Control, int TabIndex,
								  const TRect &Rect, bool Active)
{
	TCanvas* c = pc->Canvas;
	c->Brush->Style = bsSolid;
	c->Brush->Color = Active ? CL_ACCENT : CL_CARD;
	c->FillRect(Rect);
	c->Font->Color = Active ? clWhite : CL_TEXT2;
	c->Font->Style = Active ? (TFontStyles() << fsBold) : TFontStyles();
	String cap = pc->Pages[TabIndex]->Caption;
	int tw = c->TextWidth(cap), th = c->TextHeight(cap);
	c->Brush->Style = bsClear;
	c->TextOut(Rect.Left + (Rect.Width() - tw) / 2, Rect.Top + (Rect.Height() - th) / 2, cap);
}

void __fastcall TForm1::PcChange(TObject *Sender)
{
	SetEstado(L"Listo");
}

// ===========================================================================
//  PESTANA 1: CIFRAR
// ===========================================================================
void __fastcall TForm1::BtnCargar1Click(TObject *Sender)
{
	rg1->ItemIndex = 1;
	AplicarTamano(rg1);            // carga K 4x4 oficial
	memPlano1->Text = L"DELAY OPERATIONSU";
	memCifrado1->Clear();
	memDesglose1->Clear();
	SetEstado(L"Listo");
}

void __fastcall TForm1::BtnCifrarClick(TObject *Sender)
{
	try {
		SetEstado(L"Calculando...");
		int n = TamanoSel(rg1);
		Mat K = LeerMatriz(grid1);
		if (!MatrizMod26::esInvertible(K)) {
			SetEstado(L"Error: matriz no invertible", true);
			ShowMessage(L"La matriz K no es invertible mod 26. Elige otra clave.");
			return;
		}
		std::string plano = AlfabetoHill::limpiar(ToStd(memPlano1->Text));
		if (plano.empty()) {
			Fallo(L"Escribe un texto plano (solo letras A-Z).");
			return;
		}
		plano = AlfabetoHill::rellenar(plano, n);
		std::string cifrado = HillCipher::cifrar(plano, K);
		memCifrado1->Text = Agrupar(ToStr(cifrado), n);
		memDesglose1->Text = Desglose(plano, K, true);
		SetEstado(L"Listo");
	}
	catch (Exception &e)          { Fallo(e.Message); }
	catch (const std::exception &e) { Fallo(ToStr(e.what())); }
}

void __fastcall TForm1::BtnCopiarClick(TObject *Sender)
{
	try {
		Clipboard()->AsText = memCifrado1->Text;
		SetEstado(L"Texto cifrado copiado");
	}
	catch (Exception &e) { Fallo(e.Message); }
}

// ===========================================================================
//  PESTANA 2: DESCIFRAR
// ===========================================================================
void __fastcall TForm1::BtnCargar2Click(TObject *Sender)
{
	rg2->ItemIndex = 1;
	AplicarTamano(rg2);
	memCripto2->Text = L"JCOW ZLVB DVLE QMXC";
	memPlano2->Clear();
	memDesglose2->Clear();
	SetEstado(L"Listo");
}

void __fastcall TForm1::BtnDescifrarClick(TObject *Sender)
{
	try {
		SetEstado(L"Calculando...");
		int n = TamanoSel(rg2);
		Mat K = LeerMatriz(grid2);
		if (!MatrizMod26::esInvertible(K)) {
			SetEstado(L"Error: matriz no invertible", true);
			ShowMessage(L"La matriz K no es invertible mod 26. No se puede descifrar.");
			return;
		}
		Mat Kinv = GaussJordan::invertir(K);
		if (Kinv.empty()) {
			SetEstado(L"Error: matriz no invertible", true);
			ShowMessage(L"Gauss-Jordan no pudo invertir K mod 26.");
			return;
		}
		std::string cripto = AlfabetoHill::limpiar(ToStd(memCripto2->Text));
		if (cripto.empty()) {
			Fallo(L"Escribe un criptograma (solo letras A-Z).");
			return;
		}
		if ((int)cripto.size() % n != 0) {
			Fallo(String(L"La longitud del criptograma debe ser m\u00FAltiplo de ") + IntToStr(n) + L".");
			return;
		}
		memPlano2->Text = ToStr(HillCipher::descifrar(cripto, K));
		memDesglose2->Text = Desglose(cripto, Kinv, false);
		SetEstado(L"Listo");
	}
	catch (Exception &e)          { Fallo(e.Message); }
	catch (const std::exception &e) { Fallo(ToStr(e.what())); }
}

// ===========================================================================
//  PESTANA 3: CRIPTOANALISIS
// ===========================================================================
void __fastcall TForm1::BtnLlenar3Click(TObject *Sender)
{
	try {
		int n = TamanoSel(rg3);
		EscribirMatriz(gridP, BloquesAMatriz(ToStd(edtPlano3->Text), n));
		EscribirMatriz(gridC, BloquesAMatriz(ToStd(edtCripto3->Text), n));
		SetEstado(L"Listo");
	}
	catch (Exception &e)          { Fallo(e.Message); }
	catch (const std::exception &e) { Fallo(ToStr(e.what())); }
}

void __fastcall TForm1::BtnCargar3Click(TObject *Sender)
{
	rg3->ItemIndex = 1;
	AplicarTamano(rg3);
	edtPlano3->Text = L"DELAY OPERATIONSU";
	edtCripto3->Text = L"JCOW ZLVB DVLE QMXC";
	BtnLlenar3Click(Sender);
}

void __fastcall TForm1::BtnRecuperarClick(TObject *Sender)
{
	try {
		SetEstado(L"Calculando...");
		int n = TamanoSel(rg3);
		Mat P = LeerMatriz(gridP);
		Mat C = LeerMatriz(gridC);
		Mat K = GaussJordan::recuperarClave(P, C);
		if (K.empty()) {
			memK3->Clear();
			lblComp3->Caption = L"K recuperada == K original?  \u2014";
			lblComp3->Font->Color = CL_TEXT2;
			SetEstado(L"Error: matriz P no invertible", true);
			ShowMessage(L"La matriz P no es invertible mod 26; no se puede recuperar K con estos datos.");
			return;
		}
		memK3->Text = MatrizATexto(K);

		// Comparar con la K de la pestana Cifrar (si tiene el mismo tamano)
		try {
			Mat Ko = LeerMatriz(grid1);
			if ((int)Ko.size() == n) {
				bool igual = (Ko == K);
				lblComp3->Caption = String(L"K recuperada == K original?  ") + (igual ? L"SI" : L"NO");
				lblComp3->Font->Color = igual ? CL_OK : CL_ERR;
			} else {
				lblComp3->Caption = L"K recuperada == K original?  (tama\u00F1os distintos)";
				lblComp3->Font->Color = CL_TEXT2;
			}
		}
		catch (...) {
			lblComp3->Caption = L"K recuperada == K original?  \u2014";
			lblComp3->Font->Color = CL_TEXT2;
		}
		SetEstado(L"Listo");
	}
	catch (Exception &e)          { Fallo(e.Message); }
	catch (const std::exception &e) { Fallo(ToStr(e.what())); }
}

// ===========================================================================
//  PESTANA 4: INVERSA
// ===========================================================================
void __fastcall TForm1::BtnCalcInvClick(TObject *Sender)
{
	try {
		SetEstado(L"Calculando...");
		int n = TamanoSel(rg4);
		Mat K = LeerMatriz(grid4);
		if (!MatrizMod26::esInvertible(K)) {
			SetEstado(L"Error: matriz no invertible", true);
			ShowMessage(L"La matriz K no es invertible mod 26.");
			return;
		}
		Mat Kinv = GaussJordan::invertir(K);
		if (Kinv.empty()) {
			SetEstado(L"Error: matriz no invertible", true);
			ShowMessage(L"Gauss-Jordan no pudo invertir K mod 26.");
			return;
		}
		Mat I = MatrizMod26::identidad(n);
		memInv4->Text = MatrizATexto(Kinv);
		memPaso4->Text =
			String(L"Matriz aumentada inicial [K | I]:") + sLineBreak +
			AumentadaATexto(K, I) + sLineBreak + sLineBreak +
			L"Operaciones elementales por fila mod 26" + sLineBreak +
			L"(intercambio, escalado por inverso modular y eliminaci\u00F3n)" + sLineBreak +
			L"ejecutadas por GaussJordan::invertir." + sLineBreak + sLineBreak +
			L"Matriz final [I | K" S_INV L"]:" + sLineBreak +
			AumentadaATexto(I, Kinv);
		memVerif4->Clear();
		lblVerif4->Caption = L"";
		SetEstado(L"Listo");
	}
	catch (Exception &e)          { Fallo(e.Message); }
	catch (const std::exception &e) { Fallo(ToStr(e.what())); }
}

void __fastcall TForm1::BtnVerificarClick(TObject *Sender)
{
	try {
		int n = TamanoSel(rg4);
		Mat K = LeerMatriz(grid4);
		Mat Kinv = GaussJordan::invertir(K);
		if (Kinv.empty()) {
			SetEstado(L"Error: matriz no invertible", true);
			ShowMessage(L"La matriz K no es invertible mod 26.");
			return;
		}
		Mat prod = MatrizMod26::multiplicar(K, Kinv);
		memVerif4->Text = MatrizATexto(prod);
		bool esI = (prod == MatrizMod26::identidad(n));
		lblVerif4->Caption = esI ? L"K " S_DOT L" K" S_INV L" = I   \u2713"
								 : L"K " S_DOT L" K" S_INV L" \u2260 I";
		lblVerif4->Font->Color = esI ? CL_OK : CL_ERR;
		SetEstado(L"Listo");
	}
	catch (Exception &e)          { Fallo(e.Message); }
	catch (const std::exception &e) { Fallo(ToStr(e.what())); }
}
//---------------------------------------------------------------------------
