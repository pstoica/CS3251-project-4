// Code generated by Wire protocol buffer compiler, do not edit.
// Source file: ./header.proto
package com.example.myfirstapp;

import com.squareup.wire.Message;
import com.squareup.wire.ProtoEnum;
import com.squareup.wire.ProtoField;

import static com.squareup.wire.Message.Datatype.ENUM;
import static com.squareup.wire.Message.Datatype.INT32;
import static com.squareup.wire.Message.Label.REQUIRED;

public final class Header extends Message {

  public static final MethodType DEFAULT_METHOD = MethodType.LIST;
  public static final Integer DEFAULT_LENGTH = 0;
  public static final Integer DEFAULT_INDEXES = 0;

  @ProtoField(tag = 1, type = ENUM, label = REQUIRED)
  public final MethodType method;

  @ProtoField(tag = 2, type = INT32, label = REQUIRED)
  public final Integer length;

  @ProtoField(tag = 3, type = INT32, label = REQUIRED)
  public final Integer indexes;

  private Header(Builder builder) {
    super(builder);
    this.method = builder.method;
    this.length = builder.length;
    this.indexes = builder.indexes;
  }

  @Override
  public boolean equals(Object other) {
    if (other == this) return true;
    if (!(other instanceof Header)) return false;
    Header o = (Header) other;
    return equals(method, o.method)
        && equals(length, o.length)
        && equals(indexes, o.indexes);
  }

  @Override
  public int hashCode() {
    int result = hashCode;
    if (result == 0) {
      result = method != null ? method.hashCode() : 0;
      result = result * 37 + (length != null ? length.hashCode() : 0);
      result = result * 37 + (indexes != null ? indexes.hashCode() : 0);
      hashCode = result;
    }
    return result;
  }

  public static final class Builder extends Message.Builder<Header> {

    public MethodType method;
    public Integer length;
    public Integer indexes;

    public Builder() {
    }

    public Builder(Header message) {
      super(message);
      if (message == null) return;
      this.method = message.method;
      this.length = message.length;
      this.indexes = message.indexes;
    }

    public Builder method(MethodType method) {
      this.method = method;
      return this;
    }

    public Builder length(Integer length) {
      this.length = length;
      return this;
    }

    public Builder indexes(Integer indexes) {
      this.indexes = indexes;
      return this;
    }

    @Override
    public Header build() {
      checkRequiredFields();
      return new Header(this);
    }
  }

  public enum MethodType {
    @ProtoEnum(0)
    LIST,
    @ProtoEnum(1)
    DIFF,
    @ProtoEnum(2)
    PULL,
    @ProtoEnum(3)
    LEAVE,
    @ProtoEnum(4)
    CAP,
  }
}