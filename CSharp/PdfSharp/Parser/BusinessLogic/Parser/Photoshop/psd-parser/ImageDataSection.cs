﻿//Released under the MIT License.
//
//Copyright (c) 2015 Ntreev Soft co., Ltd.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
//documentation files (the "Software"), to deal in the Software without restriction, including without limitation the 
//rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
//persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the 
//Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
//COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
//OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Ntreev.Library.Psd.Readers
{
    class ImageDataSection : LazyDataReader
    {
        private Channel[] channels;

        public ImageDataSection(PsdReader reader, PsdDocument document)
        {
          Init(reader, true, document);
        }

        public Channel[] Channels
        {
            get
            {
                ReadData();
                return channels;
            }

            set
            {
                channels = value;
            }
        }

        protected override long ReadLength(PsdReader reader)
        {
            return reader.Length - reader.Position;
        }

        protected override void ReadData(PsdReader reader, object userData)
        {
            //using (MemoryStream stream = new MemoryStream(reader.ReadBytes((int)this.Length)))
            //using (PsdReader r = new PsdReader(stream, reader.Resolver, reader.Uri))
            //{
            //    r.Version = reader.Version;
            //    value = ReadValue(r, userData as PsdDocument);
            //}
            Channels = ReadValue(reader, userData as PsdDocument);
        }

        private static Channel[] ReadValue(PsdReader reader, PsdDocument document)
        {
            int channelCount = document.FileHeaderSection.NumberOfChannels;
            int width = document.Width;
            int height = document.Height;
            int depth = document.FileHeaderSection.Depth;

            CompressionType compressionType = (CompressionType)reader.ReadInt16();

            ChannelType[] types = new ChannelType[] { ChannelType.Red, ChannelType.Green, ChannelType.Blue, ChannelType.Alpha };
            Channel[] channels = new Channel[channelCount];

            for (int i = 0; i < channels.Length; i++)
            {
                ChannelType type = i < types.Length ? types[i] : ChannelType.Mask;
                channels[i] = new Channel(type, width, height, 0);
                channels[i].ReadHeader(reader, compressionType);
            }

            for (int i = 0; i < channels.Length; i++)
            {
                channels[i].Read(reader, depth, compressionType);
            }

            if (channels.Length == 4)
            {
                for (int i = 0; i < channels[3].Data.Length; i++)
                {
                    float a = channels[3].Data[i] / 255.0f;

                    for (int j = 0; j < 3; j++)
                    {
                        float r = channels[j].Data[i] / 255.0f;
                        float r1 = (((a + r) - 1f) * 1f) / a;
                        channels[j].Data[i] = (byte)(r1 * 255.0f);
                    }
                }
            }

            return channels.OrderBy(item => item.Type).ToArray();
        }
    }
}
