/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
package zykie.jimoapi;

import org.apache.thrift.scheme.IScheme;
import org.apache.thrift.scheme.SchemeFactory;
import org.apache.thrift.scheme.StandardScheme;

import org.apache.thrift.scheme.TupleScheme;
import org.apache.thrift.protocol.TTupleProtocol;
import org.apache.thrift.protocol.TProtocolException;
import org.apache.thrift.EncodingUtils;
import org.apache.thrift.TException;
import org.apache.thrift.async.AsyncMethodCallback;
import org.apache.thrift.server.AbstractNonblockingServer.*;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.EnumMap;
import java.util.Set;
import java.util.HashSet;
import java.util.EnumSet;
import java.util.Collections;
import java.util.BitSet;
import java.nio.ByteBuffer;
import java.util.Arrays;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class JmoPage implements org.apache.thrift.TBase<JmoPage, JmoPage._Fields>, java.io.Serializable, Cloneable, Comparable<JmoPage> {
  private static final org.apache.thrift.protocol.TStruct STRUCT_DESC = new org.apache.thrift.protocol.TStruct("JmoPage");

  private static final org.apache.thrift.protocol.TField START_POS_FIELD_DESC = new org.apache.thrift.protocol.TField("start_pos", org.apache.thrift.protocol.TType.I64, (short)1);
  private static final org.apache.thrift.protocol.TField SIZE_FIELD_DESC = new org.apache.thrift.protocol.TField("size", org.apache.thrift.protocol.TType.I64, (short)2);

  private static final Map<Class<? extends IScheme>, SchemeFactory> schemes = new HashMap<Class<? extends IScheme>, SchemeFactory>();
  static {
    schemes.put(StandardScheme.class, new JmoPageStandardSchemeFactory());
    schemes.put(TupleScheme.class, new JmoPageTupleSchemeFactory());
  }

  public long start_pos; // optional
  public long size; // optional

  /** The set of fields this struct contains, along with convenience methods for finding and manipulating them. */
  public enum _Fields implements org.apache.thrift.TFieldIdEnum {
    START_POS((short)1, "start_pos"),
    SIZE((short)2, "size");

    private static final Map<String, _Fields> byName = new HashMap<String, _Fields>();

    static {
      for (_Fields field : EnumSet.allOf(_Fields.class)) {
        byName.put(field.getFieldName(), field);
      }
    }

    /**
     * Find the _Fields constant that matches fieldId, or null if its not found.
     */
    public static _Fields findByThriftId(int fieldId) {
      switch(fieldId) {
        case 1: // START_POS
          return START_POS;
        case 2: // SIZE
          return SIZE;
        default:
          return null;
      }
    }

    /**
     * Find the _Fields constant that matches fieldId, throwing an exception
     * if it is not found.
     */
    public static _Fields findByThriftIdOrThrow(int fieldId) {
      _Fields fields = findByThriftId(fieldId);
      if (fields == null) throw new IllegalArgumentException("Field " + fieldId + " doesn't exist!");
      return fields;
    }

    /**
     * Find the _Fields constant that matches name, or null if its not found.
     */
    public static _Fields findByName(String name) {
      return byName.get(name);
    }

    private final short _thriftId;
    private final String _fieldName;

    _Fields(short thriftId, String fieldName) {
      _thriftId = thriftId;
      _fieldName = fieldName;
    }

    public short getThriftFieldId() {
      return _thriftId;
    }

    public String getFieldName() {
      return _fieldName;
    }
  }

  // isset id assignments
  private static final int __START_POS_ISSET_ID = 0;
  private static final int __SIZE_ISSET_ID = 1;
  private byte __isset_bitfield = 0;
  private _Fields optionals[] = {_Fields.START_POS,_Fields.SIZE};
  public static final Map<_Fields, org.apache.thrift.meta_data.FieldMetaData> metaDataMap;
  static {
    Map<_Fields, org.apache.thrift.meta_data.FieldMetaData> tmpMap = new EnumMap<_Fields, org.apache.thrift.meta_data.FieldMetaData>(_Fields.class);
    tmpMap.put(_Fields.START_POS, new org.apache.thrift.meta_data.FieldMetaData("start_pos", org.apache.thrift.TFieldRequirementType.OPTIONAL, 
        new org.apache.thrift.meta_data.FieldValueMetaData(org.apache.thrift.protocol.TType.I64)));
    tmpMap.put(_Fields.SIZE, new org.apache.thrift.meta_data.FieldMetaData("size", org.apache.thrift.TFieldRequirementType.OPTIONAL, 
        new org.apache.thrift.meta_data.FieldValueMetaData(org.apache.thrift.protocol.TType.I64)));
    metaDataMap = Collections.unmodifiableMap(tmpMap);
    org.apache.thrift.meta_data.FieldMetaData.addStructMetaDataMap(JmoPage.class, metaDataMap);
  }

  public JmoPage() {
    this.start_pos = -1L;

    this.size = -1L;

  }

  /**
   * Performs a deep copy on <i>other</i>.
   */
  public JmoPage(JmoPage other) {
    __isset_bitfield = other.__isset_bitfield;
    this.start_pos = other.start_pos;
    this.size = other.size;
  }

  public JmoPage deepCopy() {
    return new JmoPage(this);
  }

  @Override
  public void clear() {
    this.start_pos = -1L;

    this.size = -1L;

  }

  public long getStart_pos() {
    return this.start_pos;
  }

  public JmoPage setStart_pos(long start_pos) {
    this.start_pos = start_pos;
    setStart_posIsSet(true);
    return this;
  }

  public void unsetStart_pos() {
    __isset_bitfield = EncodingUtils.clearBit(__isset_bitfield, __START_POS_ISSET_ID);
  }

  /** Returns true if field start_pos is set (has been assigned a value) and false otherwise */
  public boolean isSetStart_pos() {
    return EncodingUtils.testBit(__isset_bitfield, __START_POS_ISSET_ID);
  }

  public void setStart_posIsSet(boolean value) {
    __isset_bitfield = EncodingUtils.setBit(__isset_bitfield, __START_POS_ISSET_ID, value);
  }

  public long getSize() {
    return this.size;
  }

  public JmoPage setSize(long size) {
    this.size = size;
    setSizeIsSet(true);
    return this;
  }

  public void unsetSize() {
    __isset_bitfield = EncodingUtils.clearBit(__isset_bitfield, __SIZE_ISSET_ID);
  }

  /** Returns true if field size is set (has been assigned a value) and false otherwise */
  public boolean isSetSize() {
    return EncodingUtils.testBit(__isset_bitfield, __SIZE_ISSET_ID);
  }

  public void setSizeIsSet(boolean value) {
    __isset_bitfield = EncodingUtils.setBit(__isset_bitfield, __SIZE_ISSET_ID, value);
  }

  public void setFieldValue(_Fields field, Object value) {
    switch (field) {
    case START_POS:
      if (value == null) {
        unsetStart_pos();
      } else {
        setStart_pos((Long)value);
      }
      break;

    case SIZE:
      if (value == null) {
        unsetSize();
      } else {
        setSize((Long)value);
      }
      break;

    }
  }

  public Object getFieldValue(_Fields field) {
    switch (field) {
    case START_POS:
      return Long.valueOf(getStart_pos());

    case SIZE:
      return Long.valueOf(getSize());

    }
    throw new IllegalStateException();
  }

  /** Returns true if field corresponding to fieldID is set (has been assigned a value) and false otherwise */
  public boolean isSet(_Fields field) {
    if (field == null) {
      throw new IllegalArgumentException();
    }

    switch (field) {
    case START_POS:
      return isSetStart_pos();
    case SIZE:
      return isSetSize();
    }
    throw new IllegalStateException();
  }

  @Override
  public boolean equals(Object that) {
    if (that == null)
      return false;
    if (that instanceof JmoPage)
      return this.equals((JmoPage)that);
    return false;
  }

  public boolean equals(JmoPage that) {
    if (that == null)
      return false;

    boolean this_present_start_pos = true && this.isSetStart_pos();
    boolean that_present_start_pos = true && that.isSetStart_pos();
    if (this_present_start_pos || that_present_start_pos) {
      if (!(this_present_start_pos && that_present_start_pos))
        return false;
      if (this.start_pos != that.start_pos)
        return false;
    }

    boolean this_present_size = true && this.isSetSize();
    boolean that_present_size = true && that.isSetSize();
    if (this_present_size || that_present_size) {
      if (!(this_present_size && that_present_size))
        return false;
      if (this.size != that.size)
        return false;
    }

    return true;
  }

  @Override
  public int hashCode() {
    return 0;
  }

  @Override
  public int compareTo(JmoPage other) {
    if (!getClass().equals(other.getClass())) {
      return getClass().getName().compareTo(other.getClass().getName());
    }

    int lastComparison = 0;

    lastComparison = Boolean.valueOf(isSetStart_pos()).compareTo(other.isSetStart_pos());
    if (lastComparison != 0) {
      return lastComparison;
    }
    if (isSetStart_pos()) {
      lastComparison = org.apache.thrift.TBaseHelper.compareTo(this.start_pos, other.start_pos);
      if (lastComparison != 0) {
        return lastComparison;
      }
    }
    lastComparison = Boolean.valueOf(isSetSize()).compareTo(other.isSetSize());
    if (lastComparison != 0) {
      return lastComparison;
    }
    if (isSetSize()) {
      lastComparison = org.apache.thrift.TBaseHelper.compareTo(this.size, other.size);
      if (lastComparison != 0) {
        return lastComparison;
      }
    }
    return 0;
  }

  public _Fields fieldForId(int fieldId) {
    return _Fields.findByThriftId(fieldId);
  }

  public void read(org.apache.thrift.protocol.TProtocol iprot) throws org.apache.thrift.TException {
    schemes.get(iprot.getScheme()).getScheme().read(iprot, this);
  }

  public void write(org.apache.thrift.protocol.TProtocol oprot) throws org.apache.thrift.TException {
    schemes.get(oprot.getScheme()).getScheme().write(oprot, this);
  }

  @Override
  public String toString() {
    StringBuilder sb = new StringBuilder("JmoPage(");
    boolean first = true;

    if (isSetStart_pos()) {
      sb.append("start_pos:");
      sb.append(this.start_pos);
      first = false;
    }
    if (isSetSize()) {
      if (!first) sb.append(", ");
      sb.append("size:");
      sb.append(this.size);
      first = false;
    }
    sb.append(")");
    return sb.toString();
  }

  public void validate() throws org.apache.thrift.TException {
    // check for required fields
    // check for sub-struct validity
  }

  private void writeObject(java.io.ObjectOutputStream out) throws java.io.IOException {
    try {
      write(new org.apache.thrift.protocol.TCompactProtocol(new org.apache.thrift.transport.TIOStreamTransport(out)));
    } catch (org.apache.thrift.TException te) {
      throw new java.io.IOException(te);
    }
  }

  private void readObject(java.io.ObjectInputStream in) throws java.io.IOException, ClassNotFoundException {
    try {
      // it doesn't seem like you should have to do this, but java serialization is wacky, and doesn't call the default constructor.
      __isset_bitfield = 0;
      read(new org.apache.thrift.protocol.TCompactProtocol(new org.apache.thrift.transport.TIOStreamTransport(in)));
    } catch (org.apache.thrift.TException te) {
      throw new java.io.IOException(te);
    }
  }

  private static class JmoPageStandardSchemeFactory implements SchemeFactory {
    public JmoPageStandardScheme getScheme() {
      return new JmoPageStandardScheme();
    }
  }

  private static class JmoPageStandardScheme extends StandardScheme<JmoPage> {

    public void read(org.apache.thrift.protocol.TProtocol iprot, JmoPage struct) throws org.apache.thrift.TException {
      org.apache.thrift.protocol.TField schemeField;
      iprot.readStructBegin();
      while (true)
      {
        schemeField = iprot.readFieldBegin();
        if (schemeField.type == org.apache.thrift.protocol.TType.STOP) { 
          break;
        }
        switch (schemeField.id) {
          case 1: // START_POS
            if (schemeField.type == org.apache.thrift.protocol.TType.I64) {
              struct.start_pos = iprot.readI64();
              struct.setStart_posIsSet(true);
            } else { 
              org.apache.thrift.protocol.TProtocolUtil.skip(iprot, schemeField.type);
            }
            break;
          case 2: // SIZE
            if (schemeField.type == org.apache.thrift.protocol.TType.I64) {
              struct.size = iprot.readI64();
              struct.setSizeIsSet(true);
            } else { 
              org.apache.thrift.protocol.TProtocolUtil.skip(iprot, schemeField.type);
            }
            break;
          default:
            org.apache.thrift.protocol.TProtocolUtil.skip(iprot, schemeField.type);
        }
        iprot.readFieldEnd();
      }
      iprot.readStructEnd();

      // check for required fields of primitive type, which can't be checked in the validate method
      struct.validate();
    }

    public void write(org.apache.thrift.protocol.TProtocol oprot, JmoPage struct) throws org.apache.thrift.TException {
      struct.validate();

      oprot.writeStructBegin(STRUCT_DESC);
      if (struct.isSetStart_pos()) {
        oprot.writeFieldBegin(START_POS_FIELD_DESC);
        oprot.writeI64(struct.start_pos);
        oprot.writeFieldEnd();
      }
      if (struct.isSetSize()) {
        oprot.writeFieldBegin(SIZE_FIELD_DESC);
        oprot.writeI64(struct.size);
        oprot.writeFieldEnd();
      }
      oprot.writeFieldStop();
      oprot.writeStructEnd();
    }

  }

  private static class JmoPageTupleSchemeFactory implements SchemeFactory {
    public JmoPageTupleScheme getScheme() {
      return new JmoPageTupleScheme();
    }
  }

  private static class JmoPageTupleScheme extends TupleScheme<JmoPage> {

    @Override
    public void write(org.apache.thrift.protocol.TProtocol prot, JmoPage struct) throws org.apache.thrift.TException {
      TTupleProtocol oprot = (TTupleProtocol) prot;
      BitSet optionals = new BitSet();
      if (struct.isSetStart_pos()) {
        optionals.set(0);
      }
      if (struct.isSetSize()) {
        optionals.set(1);
      }
      oprot.writeBitSet(optionals, 2);
      if (struct.isSetStart_pos()) {
        oprot.writeI64(struct.start_pos);
      }
      if (struct.isSetSize()) {
        oprot.writeI64(struct.size);
      }
    }

    @Override
    public void read(org.apache.thrift.protocol.TProtocol prot, JmoPage struct) throws org.apache.thrift.TException {
      TTupleProtocol iprot = (TTupleProtocol) prot;
      BitSet incoming = iprot.readBitSet(2);
      if (incoming.get(0)) {
        struct.start_pos = iprot.readI64();
        struct.setStart_posIsSet(true);
      }
      if (incoming.get(1)) {
        struct.size = iprot.readI64();
        struct.setSizeIsSet(true);
      }
    }
  }

}

