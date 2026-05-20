#include <sheen/core/concepts.hpp>

#include <catch2/catch_test_macros.hpp>

namespace {

struct out_ar {
  void operator()(int const&) {}
};
struct in_ar {
  void operator()(int&) {}
};

struct member_save_only {
  template<class A>
  void save(A&) const
  {
  }
};
struct member_load_only {
  template<class A>
  void load(A&)
  {
  }
};
struct member_serialize_const {
  template<class A>
  void serialize(A&) const
  {
  }
};
struct member_serialize_mut {
  template<class A>
  void serialize(A&)
  {
  }
};
struct member_serialize_symmetric {
  template<class Self, class A>
  void serialize(this Self&&, A&)
  {
  }
};

struct nothing {};

}  // namespace

namespace fsave_ns {
struct fsave_only {};
template<class A>
void save(A&, fsave_only const&)
{
}
}  // namespace fsave_ns

namespace fload_ns {
struct fload_only {};
template<class A>
void load(A&, fload_only&)
{
}
}  // namespace fload_ns

namespace fserialize_out_ns {
struct fserialize_out {};
template<class A>
void serialize(A&, fserialize_out const&)
{
}
}  // namespace fserialize_out_ns

namespace fserialize_in_ns {
struct fserialize_in {};
template<class A>
void serialize(A&, fserialize_in&)
{
}
}  // namespace fserialize_in_ns

TEST_CASE("intrusive output customization points")
{
  using namespace sheen::detail;

  STATIC_CHECK(has_output_member_save<out_ar, member_save_only>);
  STATIC_CHECK(!has_output_member_serialize<out_ar, member_save_only>);
  STATIC_CHECK(!has_output_free_save<out_ar, member_save_only>);
  STATIC_CHECK(!has_output_free_serialize<out_ar, member_save_only>);

  STATIC_CHECK(has_output_member_serialize<out_ar, member_serialize_const>);
  STATIC_CHECK(!has_output_member_save<out_ar, member_serialize_const>);

  STATIC_CHECK(!has_output_member_serialize<out_ar, member_serialize_mut>);

  STATIC_CHECK(has_output_member_serialize<out_ar, member_serialize_symmetric>);

  STATIC_CHECK(!has_output_member_save<out_ar, nothing>);
  STATIC_CHECK(!has_output_member_serialize<out_ar, nothing>);
}

TEST_CASE("free (ADL) output customization points")
{
  using namespace sheen::detail;

  STATIC_CHECK(has_output_free_save<out_ar, fsave_ns::fsave_only>);
  STATIC_CHECK(!has_output_member_save<out_ar, fsave_ns::fsave_only>);

  STATIC_CHECK(has_output_free_serialize<out_ar, fserialize_out_ns::fserialize_out>);
  STATIC_CHECK(!has_output_free_save<out_ar, fserialize_out_ns::fserialize_out>);
}

TEST_CASE("intrusive input customization points")
{
  using namespace sheen::detail;

  STATIC_CHECK(has_input_member_load<in_ar, member_load_only&>);
  STATIC_CHECK(!has_input_member_serialize<in_ar, member_load_only&>);
  STATIC_CHECK(!has_input_free_load<in_ar, member_load_only&>);

  STATIC_CHECK(has_input_member_serialize<in_ar, member_serialize_mut&>);
  STATIC_CHECK(!has_input_member_load<in_ar, member_serialize_mut&>);

  STATIC_CHECK(has_input_member_serialize<in_ar, member_serialize_const&>);

  STATIC_CHECK(has_input_member_serialize<in_ar, member_serialize_symmetric&>);

  STATIC_CHECK(!has_input_member_load<in_ar, nothing&>);
  STATIC_CHECK(!has_input_member_serialize<in_ar, nothing&>);
}

TEST_CASE("free (ADL) input customization points")
{
  using namespace sheen::detail;

  STATIC_CHECK(has_input_free_load<in_ar, fload_ns::fload_only&>);
  STATIC_CHECK(!has_input_member_load<in_ar, fload_ns::fload_only&>);

  STATIC_CHECK(has_input_free_serialize<in_ar, fserialize_in_ns::fserialize_in&>);
  STATIC_CHECK(!has_input_free_load<in_ar, fserialize_in_ns::fserialize_in&>);
}
