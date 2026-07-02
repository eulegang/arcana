#include "arcana/types.h"
#include <gtest/gtest.h>

using namespace arcana::types;

TEST(type_id, eq) {
  EXPECT_EQ(type_id(type_id::cat::meta, 0), type_id(type_id::cat::meta, 0));
  EXPECT_NE(type_id(type_id::cat::meta, 0), type_id(type_id::cat::meta, 1));
  EXPECT_NE(type_id(type_id::cat::meta, 0), type_id(type_id::cat::st, 0));
}

TEST(type_id, std_instances) {
  EXPECT_EQ(type_id(), type_id(type_id::cat::meta, 0));

  EXPECT_EQ(type_id::null, type_id(type_id::cat::meta, 0));
  EXPECT_EQ(type_id::poison, type_id(type_id::cat::meta, 1));

  EXPECT_EQ(type_id::unit, type_id(type_id::cat::prim, 0));
  EXPECT_EQ(type_id::boolean, type_id(type_id::cat::prim, 1));
}

TEST(type_id, casts) {
  EXPECT_FALSE(type_id::null);
  EXPECT_TRUE(type_id::boolean);
}

TEST(type_id, inc) {
  type_id id = type_id(type_id::cat::st, 0);

  type_id pre = ++id;
  EXPECT_EQ(pre, id);
  EXPECT_EQ(id, type_id(type_id::cat::st, 1));

  type_id post = id++;
  EXPECT_EQ(pre, post);
  EXPECT_EQ(id, type_id(type_id::cat::st, 2));

  EXPECT_LT(pre, id);
}
