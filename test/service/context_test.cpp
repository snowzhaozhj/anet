#include <anet/service/context.hpp>

#include "anet_test.hpp"

#include <memory>

namespace anet {

class ContextTest : public testing::Test {
 public:
  Context context_;
};

struct Dummy {
  int a;
  std::string msg;
  float m;
};

TEST_F(ContextTest, Base) {
  context_.Set("Hello", 1);
  EXPECT_EQ(*context_.Get<int>("Hello"), 1);
  EXPECT_EQ(context_.Get<int>("hi"), nullptr);

  std::string a = "world";
  context_.Set("a", a);
  EXPECT_EQ(*context_.Get<std::string>("a"), a);

  std::map<std::string, std::string> m;
  m["hello"] = "k";
  context_.Set("m", std::move(m));
  auto &stored_m = *context_.Get<std::map<std::string, std::string>>("m");
  EXPECT_EQ(stored_m["hello"], "k");

  Dummy d1;
  d1.m = 1.1;
  d1.a = 5;
  d1.msg = "hello";
  context_.Set("d1", std::move(d1));
  Dummy *stored_d1 = context_.Get<Dummy>("d1");
  EXPECT_FLOAT_EQ(stored_d1->m, 1.1);
  EXPECT_EQ(stored_d1->msg, "hello");
  EXPECT_EQ(stored_d1->a, 5);
}

class NoncopyObject {
 public:
  NoncopyObject() = default;

  NoncopyObject(const NoncopyObject &) = delete;
  NoncopyObject &operator=(const NoncopyObject &) = delete;

  std::string msg;
};

TEST_F(ContextTest, Noncopy) {
  auto ptr_obj = new NoncopyObject;
  ptr_obj->msg = "Hello";
  context_.Set("ptr_obj", ptr_obj);
  auto stored_ptr_obj = *context_.Get<NoncopyObject *>("ptr_obj");
  EXPECT_EQ(stored_ptr_obj->msg, "Hello");
  delete ptr_obj;

  auto shared_obj = std::make_shared<NoncopyObject>();
  shared_obj->msg = "test";
  context_.Set("shared_obj", shared_obj);
  EXPECT_EQ(shared_obj.use_count(), 2);
  auto &stored_shared_obj = *context_.Get<std::shared_ptr<NoncopyObject>>("shared_obj");
  EXPECT_EQ(shared_obj.use_count(), 2);
  EXPECT_EQ(stored_shared_obj->msg, "test");
  stored_shared_obj.reset();
  EXPECT_EQ(shared_obj.use_count(), 1);
}

} // namespace anet
