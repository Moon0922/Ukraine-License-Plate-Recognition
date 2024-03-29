#include "pch.h"
#include "xMemDC.h"


CxMemDC::CxMemDC(CDC* pDC, int nWidth, int nHeight)
{
	CreateCompatibleDC(pDC);

	m_cbmMem.CreateCompatibleBitmap(pDC, nWidth, nHeight);

	m_pcbmOld = SelectObject(&m_cbmMem);
}


CxMemDC::~CxMemDC()
{
	SelectObject(m_pcbmOld);
	m_cbmMem.DeleteObject();
	DeleteDC();
}

void CxMemDC::DrawTheRectangle(CRect crcArea, COLORREF crRect, COLORREF crEdge)
{
	CBrush brush;
	brush.CreateSolidBrush(crRect);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, crEdge);

	CBrush* pOldBrush = SelectObject(&brush);
	CPen* pOldPen = SelectObject(&pen);


	Rectangle(crcArea);

	SelectObject(pOldPen);
	SelectObject(pOldBrush);
}

void CxMemDC::DrawTheText(CString strText, CRect crcArea, UINT nFormat, CFont* pFont, COLORREF crText)
{
	SetBkMode(TRANSPARENT);
	SetTextColor(crText);
	CFont* pOldFont = SelectObject(pFont);

	DrawText(strText, crcArea, nFormat);

	SelectObject(pOldFont);
}

void CxMemDC::DrawTheLine(int x1, int y1, int x2, int y2, COLORREF cr, int nWidth)
{
	CPen pen;
	pen.CreatePen(PS_SOLID, nWidth, cr);

	CPen* pOldPen = SelectObject(&pen);

	MoveTo(x1, y1);
	LineTo(x2, y2);

	SelectObject(pOldPen);
}

int _cdecl ptFuncCompare(const void* aa1, const void* aa2)
{
	int a1 = *(int*)aa1;
	int a2 = *(int*)aa2;

	if (a1 > a2)
		return 1;
	else if (a1 < a2)
		return -1;

	return 0;
}

void CxMemDC::DrawBlt(CDC* pDC, CRect crcBlt)
{
	CWnd* pWnd = pDC->GetWindow();

	int nChildCount = 0;
	CRect crcChild[100];

	if (pWnd)
	{
		pWnd->GetClientRect(&crcChild[0]);
		nChildCount = 1;
	}


	pWnd = pWnd->GetWindow(GW_CHILD);
	while (pWnd)
	{
		if (pWnd->IsWindowVisible())
		{
			CRect crcClient;
			pWnd->GetWindowRect(&crcClient);

			pDC->GetWindow()->ScreenToClient(&crcClient);

			crcChild[nChildCount] = crcClient;
			nChildCount++;
		}
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}

	int nXCount = 0, nYCount = 0;
	int nX[100], nY[100];

	for (int i = 0; i < nChildCount; i++)
	{
		nX[nXCount] = crcChild[i].left; nXCount++;
		nY[nYCount] = crcChild[i].top; nYCount++;
		nX[nXCount] = crcChild[i].right; nXCount++;
		nY[nYCount] = crcChild[i].bottom; nYCount++;
	}

	qsort(nX, nXCount, sizeof(int), ptFuncCompare);
	qsort(nY, nYCount, sizeof(int), ptFuncCompare);

	for (int i = 0; i < nXCount - 1; i++)
	{
		for (int j = 0; j < nYCount - 1; j++)
		{

			int k = 1;
			for (k = 1; k < nChildCount; k++)
			{
				if (PtInRect(crcChild[k], CPoint(nX[i], nY[j])) )
				{
					break;
				}
			}

			if (k == nChildCount)
			{
				pDC->BitBlt(nX[i], nY[j], nX[i + 1] - nX[i], nY[j + 1] - nY[j], this, nX[i], nY[j], SRCCOPY);
			}
		}
	}
}

void CxMemDC::DrawTheDotRect(CRect crcData, COLORREF crText)
{
	CBrush brush;
	brush.CreateStockObject(NULL_BRUSH);

	CBrush* pOldBrush = SelectObject(&brush);

	CPen pen;
	pen.CreatePen(PS_DOT, 1, crText);

	CPen* pOldPen = SelectObject(&pen);

	Rectangle(crcData);

	SelectObject(pOldPen);
	SelectObject(pOldBrush);
}

void CxMemDC::DrawTheCirCle(int x, int y, int r, COLORREF crArea, COLORREF crEdge)
{
	CBrush brush;
	brush.CreateSolidBrush(crArea);

	CBrush* pOldBrush = SelectObject(&brush);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, crEdge);

	CPen* pOldPen = SelectObject(&pen);
	Ellipse(x - r, y - r, x + r, y + r);


	SelectObject(pOldBrush);
	SelectObject(pOldPen);
}

void CxMemDC::DrawThePng(int nResId, CRect crcPng)
{
	HRSRC hSrc = FindResource(0, MAKEINTRESOURCE(nResId), _T("PNG"));
	HGLOBAL hRes = LoadResource(0, hSrc);

	LPBYTE pImage = (LPBYTE) LockResource(hRes);
	int nLen = SizeofResource(0, hSrc);
	
	CxImage image(pImage, nLen, CXIMAGE_FORMAT_PNG);

	image.Draw( GetSafeHdc(), crcPng);

	UnlockResource(hRes);
	FreeResource(hRes);
}

int CxMemDC::DrawTheTextEx(int nY, CString strText, CRect crcText, CFont* pFont, COLORREF crText)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(this);

	CBitmap cbmMem;
	cbmMem.CreateCompatibleBitmap(this, crcText.Width(), crcText.Height());

	CBitmap* pcbmOld = dcMem.SelectObject(&cbmMem);

	dcMem.BitBlt(0, 0, crcText.Width(), crcText.Height(), this, crcText.left, crcText.top, SRCCOPY);
	CFont* pOldFont = dcMem.SelectObject(pFont);


	int nLen = strText.GetLength();

	LPWSTR lpwchText = new WCHAR[nLen + 1];
	nLen = MultiByteToWideChar(950, 0, (char*) (LPCTSTR) strText, nLen, lpwchText, nLen);
	lpwchText[nLen] = 0;

	dcMem.SetBkMode(TRANSPARENT);
	dcMem.SetTextColor(crText);

	int nSizeX = 0, nSizeY = 0;
	for(int i = 0; i < nLen; i++)
	{
		if( lpwchText[i] == 0x0d && lpwchText[i + 1] == 0xa )
		{
			nSizeX = 0;
			nSizeY += 30;
			i++;
		}
		else if( lpwchText[i] == 0x0d || lpwchText[i] == 0xa )
		{
			nSizeX = crcText.left;
			nSizeY += 30;
		}
		else 
		{
			CSize sz;
			::GetTextExtentPoint32W( dcMem.GetSafeHdc(), lpwchText + i, 1, &sz);

			if( nSizeX + sz.cx > crcText.Width() )
			{
				nSizeX = crcText.left;
				nSizeY += 30;
			}

			nSizeX += sz.cx;
		}
	}
	nSizeY += 30;


	if( nSizeY < crcText.Height() )
	{
		nY = (crcText.Height() - nSizeY) / 2;
	}
	else
	{
	}

	int nX = crcText.left;
	nY += crcText.top;
	for(int i = 0; i < nLen; i++)
	{
		if( lpwchText[i] == 0x0d && lpwchText[i + 1] == 0xa )
		{
			nX = crcText.left;
			nY += 30;
			i++;
		}
		else if( lpwchText[i] == 0x0d || lpwchText[i] == 0xa )
		{
			nX = crcText.left;
			nY += 30;
		}
		else 
		{
			CSize sz;
			::GetTextExtentPoint32W( dcMem.GetSafeHdc(), lpwchText + i, 1, &sz);

			if( nX + sz.cx > crcText.right )
			{
				nX = crcText.left;
				nY += 30;
			}

			::TextOutW( dcMem.GetSafeHdc(), nX - crcText.left, nY - crcText.top, lpwchText + i, 1);
			nX += sz.cx;
		}
	}

	nY += 30;

	delete lpwchText;

	BitBlt(crcText.left, crcText.top, crcText.Width(), crcText.Height(), &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject(pOldFont);
	dcMem.SelectObject(pcbmOld);

	return nSizeY;
}