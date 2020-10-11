﻿// StockAnalysisDoc.h: CStockAnalysisDoc 类的接口
//

#pragma once

class CStockAnalysisDoc : public CDocument
{
protected: // 仅从序列化创建
  CStockAnalysisDoc();
  DECLARE_DYNCREATE(CStockAnalysisDoc)

  // 特性
public:

  // 操作
public:

  // 重写
public:
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDayLineERS
  virtual void InitializeSearchContent();
  virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDayLineERS

  // 实现
public:
  virtual ~CStockAnalysisDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:

  // 生成的消息映射函数
protected:
  DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDayLineERS
  // 用于为搜索处理程序设置搜索内容的 Helper 函数
  void SetSearchContent(const CString& value);
#endif // SHARED_HANDayLineERS
};
