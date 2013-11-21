// Code generated by Wire protocol buffer compiler, do not edit.
// Source file: ./song.proto
package com.example.myfirstapp;

import com.squareup.wire.Message;
import com.squareup.wire.ProtoField;

import static com.squareup.wire.Message.Datatype.INT32;
import static com.squareup.wire.Message.Datatype.STRING;
import static com.squareup.wire.Message.Label.REQUIRED;

public final class Song extends Message {

  public static final String DEFAULT_TITLE = "";
  public static final String DEFAULT_CHECKSUM = "";
  public static final Integer DEFAULT_LENOFSONG = 0;

  @ProtoField(tag = 1, type = STRING, label = REQUIRED)
  public final String title;

  @ProtoField(tag = 2, type = STRING, label = REQUIRED)
  public final String checksum;

  @ProtoField(tag = 3, type = INT32, label = REQUIRED)
  public final Integer lenOfSong;

  private Song(Builder builder) {
    super(builder);
    this.title = builder.title;
    this.checksum = builder.checksum;
    this.lenOfSong = builder.lenOfSong;
  }

  @Override
  public boolean equals(Object other) {
    if (other == this) return true;
    if (!(other instanceof Song)) return false;
    Song o = (Song) other;
    return equals(title, o.title)
        && equals(checksum, o.checksum)
        && equals(lenOfSong, o.lenOfSong);
  }

  @Override
  public int hashCode() {
    int result = hashCode;
    if (result == 0) {
      result = title != null ? title.hashCode() : 0;
      result = result * 37 + (checksum != null ? checksum.hashCode() : 0);
      result = result * 37 + (lenOfSong != null ? lenOfSong.hashCode() : 0);
      hashCode = result;
    }
    return result;
  }

  public static final class Builder extends Message.Builder<Song> {

    public String title;
    public String checksum;
    public Integer lenOfSong;

    public Builder() {
    }

    public Builder(Song message) {
      super(message);
      if (message == null) return;
      this.title = message.title;
      this.checksum = message.checksum;
      this.lenOfSong = message.lenOfSong;
    }

    public Builder title(String title) {
      this.title = title;
      return this;
    }

    public Builder checksum(String checksum) {
      this.checksum = checksum;
      return this;
    }

    public Builder lenOfSong(Integer lenOfSong) {
      this.lenOfSong = lenOfSong;
      return this;
    }

    @Override
    public Song build() {
      checkRequiredFields();
      return new Song(this);
    }
  }
}
