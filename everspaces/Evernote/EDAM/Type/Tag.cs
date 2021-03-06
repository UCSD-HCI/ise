/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 */
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using Thrift;
using Thrift.Collections;
using Thrift.Protocol;
using Thrift.Transport;
namespace Evernote.EDAM.Type
{

  #if !SILVERLIGHT
  [Serializable]
  #endif
  public partial class Tag : TBase
  {
    private string _guid;
    private string _name;
    private string _parentGuid;
    private int _updateSequenceNum;

    public string Guid
    {
      get
      {
        return _guid;
      }
      set
      {
        __isset.guid = true;
        this._guid = value;
      }
    }

    public string Name
    {
      get
      {
        return _name;
      }
      set
      {
        __isset.name = true;
        this._name = value;
      }
    }

    public string ParentGuid
    {
      get
      {
        return _parentGuid;
      }
      set
      {
        __isset.parentGuid = true;
        this._parentGuid = value;
      }
    }

    public int UpdateSequenceNum
    {
      get
      {
        return _updateSequenceNum;
      }
      set
      {
        __isset.updateSequenceNum = true;
        this._updateSequenceNum = value;
      }
    }


    public Isset __isset;
    #if !SILVERLIGHT
    [Serializable]
    #endif
    public struct Isset {
      public bool guid;
      public bool name;
      public bool parentGuid;
      public bool updateSequenceNum;
    }

    public Tag() {
    }

    public void Read (TProtocol iprot)
    {
      TField field;
      iprot.ReadStructBegin();
      while (true)
      {
        field = iprot.ReadFieldBegin();
        if (field.Type == TType.Stop) { 
          break;
        }
        switch (field.ID)
        {
          case 1:
            if (field.Type == TType.String) {
              Guid = iprot.ReadString();
            } else { 
              TProtocolUtil.Skip(iprot, field.Type);
            }
            break;
          case 2:
            if (field.Type == TType.String) {
              Name = iprot.ReadString();
            } else { 
              TProtocolUtil.Skip(iprot, field.Type);
            }
            break;
          case 3:
            if (field.Type == TType.String) {
              ParentGuid = iprot.ReadString();
            } else { 
              TProtocolUtil.Skip(iprot, field.Type);
            }
            break;
          case 4:
            if (field.Type == TType.I32) {
              UpdateSequenceNum = iprot.ReadI32();
            } else { 
              TProtocolUtil.Skip(iprot, field.Type);
            }
            break;
          default: 
            TProtocolUtil.Skip(iprot, field.Type);
            break;
        }
        iprot.ReadFieldEnd();
      }
      iprot.ReadStructEnd();
    }

    public void Write(TProtocol oprot) {
      TStruct struc = new TStruct("Tag");
      oprot.WriteStructBegin(struc);
      TField field = new TField();
      if (Guid != null && __isset.guid) {
        field.Name = "guid";
        field.Type = TType.String;
        field.ID = 1;
        oprot.WriteFieldBegin(field);
        oprot.WriteString(Guid);
        oprot.WriteFieldEnd();
      }
      if (Name != null && __isset.name) {
        field.Name = "name";
        field.Type = TType.String;
        field.ID = 2;
        oprot.WriteFieldBegin(field);
        oprot.WriteString(Name);
        oprot.WriteFieldEnd();
      }
      if (ParentGuid != null && __isset.parentGuid) {
        field.Name = "parentGuid";
        field.Type = TType.String;
        field.ID = 3;
        oprot.WriteFieldBegin(field);
        oprot.WriteString(ParentGuid);
        oprot.WriteFieldEnd();
      }
      if (__isset.updateSequenceNum) {
        field.Name = "updateSequenceNum";
        field.Type = TType.I32;
        field.ID = 4;
        oprot.WriteFieldBegin(field);
        oprot.WriteI32(UpdateSequenceNum);
        oprot.WriteFieldEnd();
      }
      oprot.WriteFieldStop();
      oprot.WriteStructEnd();
    }

    public override string ToString() {
      StringBuilder sb = new StringBuilder("Tag(");
      sb.Append("Guid: ");
      sb.Append(Guid);
      sb.Append(",Name: ");
      sb.Append(Name);
      sb.Append(",ParentGuid: ");
      sb.Append(ParentGuid);
      sb.Append(",UpdateSequenceNum: ");
      sb.Append(UpdateSequenceNum);
      sb.Append(")");
      return sb.ToString();
    }

  }

}
