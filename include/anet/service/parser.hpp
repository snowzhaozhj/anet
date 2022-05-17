#ifndef ANET_INCLUDE_ANET_SERVICE_PARSER_HPP_
#define ANET_INCLUDE_ANET_SERVICE_PARSER_HPP_

#include <any>
#include <memory>
#include <string_view>

namespace anet {

class Parser {
 public:
  Parser() = default;
  virtual ~Parser() = default;

  [[nodiscard]] virtual int GetInitReadSize() const { return 0; }

  /// @return
  /// -2�����������
  /// -1��ʾ�������
  /// 0��ʾ����Ҫ�������ݣ����޷���֪������Ҫ��������
  /// n(n>0)��ʾ����Ҫn�ֽڵ�����
  int Parse(std::string_view data) { return Parse(data.data(), data.size()); }
  virtual int Parse(const char *data, std::size_t len) = 0;

  void SetObject(std::any object) { object_ = std::move(object); }
  std::any &GetObject() { return object_; }

 private:
  std::any object_;
};

using ParserPtr = std::shared_ptr<Parser>;

} // namespace anet

#endif //ANET_INCLUDE_ANET_SERVICE_PARSER_HPP_
