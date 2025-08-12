#include "CalcDisplay.h"
#include "iostream"
#include <emscripten.h>
#include <codecvt>
#include <locale>
#include <regex> 

extern "C" {

EM_JS(void, setPr, (char* dStr), { 
    window.setPrimaryDisplay(UTF8ToString(dStr));
});
EM_JS(void, setEx, (char* eStr), {
    window.setExpressionDisplay(UTF8ToString(eStr));
});
EM_JS(void, setHs, (char* ex, char* res), {
    window.setHistoryItem(UTF8ToString(ex), UTF8ToString(res));
});
EM_JS(void, setMem, (char* mem), {
    window.setMemoryItem(UTF8ToString(mem));
});
EM_JS(void, upMem, (char* mem, int idx), {
    window.updateMemoryItem(UTF8ToString(mem), idx);
})


}

// Helper function to fix sqrt symbol not showing in calculator
std::string wstring_to_utf8(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(str);
}

CalcDisplay::CalcDisplay() {
  nMemItems = 0;
}

void CalcDisplay::SetStdCallback(
  CalculationManager::CalculatorManager* standardCalculatorManagerPtr) {
  m_standardCalculatorManagerPtr = standardCalculatorManagerPtr;
}

void CalcDisplay::SetPrimaryDisplay(const std::wstring &pszText, bool isError)
{
    char* str = new char[255];
    sprintf(str, "%ls", pszText.c_str());
    setPr(str);
}
void CalcDisplay::SetIsInError(bool isInError)
{
    //Nothing for now
}
void CalcDisplay::SetExpressionDisplay(
    _Inout_ std::shared_ptr<std::vector<std::pair<std::wstring, int>>> const &tokens,
    _Inout_ std::shared_ptr<std::vector<std::shared_ptr<IExpressionCommand>>> const &commands)
{
  auto ntokens = tokens->size();
  std::wstring exDisplay;
  for (auto i = 0; i < ntokens; ++i) {
    auto currentToken = (*tokens)[i];
    auto currentTokenString = currentToken.first;
    exDisplay += currentTokenString;
  }
  
  std::string utf8String = wstring_to_utf8(exDisplay);
  char* str = new char[utf8String.length() + 1];
  strcpy(str, utf8String.c_str());
  setEx(str);
  delete[] str;
}
void CalcDisplay::SetParenthesisNumber(_In_ unsigned int count)
{
    //std::wcout << "<<SetParenthesis not implemented yet>>" << std::endl;
}
void CalcDisplay::OnNoRightParenAdded()
{
    std::wcout << "<<OnRightParenAdded not implemented yet>>" << std::endl;
}
void CalcDisplay::MaxDigitsReached()
{
    //do nothing
} // not an error but still need to inform UI layer.
void CalcDisplay::BinaryOperatorReceived()
{
    //Do Nothing
}
void CalcDisplay::OnHistoryItemAdded(_In_ unsigned int addedItemIndex)
{
  auto hItemV = m_standardCalculatorManagerPtr->GetHistoryItem(addedItemIndex)->historyItemVector;
  auto expression = hItemV.expression;
  auto result = hItemV.result;
  expression = expression.substr(0, expression.length() - 1);

  expression = std::regex_replace(expression, std::wregex(L"\\( "), L"(");
  expression = std::regex_replace(expression, std::wregex(L" \\)"), L")");
  
  std::string exprUtf8 = wstring_to_utf8(expression);
  std::string resultUtf8 = wstring_to_utf8(result);
  
  char* ex = new char[exprUtf8.length() + 1];
  char* res = new char[resultUtf8.length() + 1];
  strcpy(ex, exprUtf8.c_str());
  strcpy(res, resultUtf8.c_str());
  setHs(ex, res);
  delete[] ex;
  delete[] res;
}
void CalcDisplay::SetMemorizedNumbers(const std::vector<std::wstring> &memorizedNumbers)
{
  m_memorizedNumbers = memorizedNumbers;
  auto len = memorizedNumbers.size();
  auto num = memorizedNumbers[0];
  char* str = new char[255];
  sprintf(str, "%ls", num.c_str());
  if(len > nMemItems) {
    setMem(str);
    nMemItems++;
  }
}
void CalcDisplay::MemoryItemChanged(unsigned int indexOfMemory)
{
  auto mem = m_memorizedNumbers[indexOfMemory];
  char* str = new char[255];
  sprintf(str, "%ls", mem.c_str());
  upMem(str, indexOfMemory);
}
void CalcDisplay::InputChanged()
{
    //yes i sent that
}