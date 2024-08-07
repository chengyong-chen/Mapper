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
using System.Runtime.CompilerServices;
using System.Linq;
using Ntreev.Library.Psd.Readers;
using Ntreev.Library.Psd.Readers.LayerAndMaskInformation;
using Ntreev.Library.Psd.Readers.LayerResources;

namespace Ntreev.Library.Psd
{
    class PsdLayer : IPsdLayer
    {
        private readonly PsdDocument document;
        private readonly LayerRecords records;

        private int left, top, right, bottom;

        private PsdLayer[] childreen;
        private PsdLayer parent;
        private ILinkedLayer linkedLayer;

        private ChannelsBlock channels;

        private static PsdLayer[] emptyChildreen = new PsdLayer[] { };

        public PsdLayer(PsdReader reader, PsdDocument document)
        {
            this.document = document;
            this.records = LayerRecordsBlock.Read(reader);
            this.records = LayerExtraRecordsBlock.Read(reader, this.records);

            this.left = this.records.Left;
            this.top = this.records.Top;
            this.right = this.records.Right;
            this.bottom = this.records.Bottom;
        }

        public override string ToString()
        {
            return this.Name;
        }

        public Channel[] Channels
        {
            get { return this.channels.Channels; }
        }

        public SectionType SectionType
        {
            get { return this.records.SectionType; }
        }

        public string Name
        {
            get { return this.records.Name; }
        }

        public bool IsVisible
        {
            get { return (this.records.Flags & LayerFlags.Visible) != LayerFlags.Visible; }
        }

        public float Opacity
        {
            get { return (((float)this.records.Opacity) / 255f); }
        }

        public int Left
        {
            get { return this.left; }
        }

        public int Top
        {
            get { return this.top; }
        }

        public int Right
        {
            get { return this.right; }
        }

        public int Bottom
        {
            get { return this.bottom; }
        }

        public int Width
        {
            get { return this.right - this.left; }
        }

        public int Height
        {
            get { return this.bottom - this.top; }
        }

        public int Depth
        {
            get { return this.document.FileHeaderSection.Depth; }
        }

        public bool IsClipping
        {
            get { return this.records.Clipping; }
        }

        public BlendMode BlendMode
        {
            get { return this.records.BlendMode; }
        }

        public PsdLayer Parent
        {
            get { return this.parent; }
            set { this.parent = value; }
        }

        public PsdLayer[] Childreen
        {
            get
            {
                if (this.childreen == null)
                    return emptyChildreen;
                return this.childreen;
            }
            set { this.childreen = value; }
        }

        public IResourceBlock[] Resources
        {
            get { return this.records.Resources; }
        }

        public PsdDocument Document
        {
            get { return this.document; }
        }

        public LayerRecords Records
        {
            get { return this.records; }
        }

        public ILinkedLayer LinkedLayer
        {
            get
            {
                Guid placeID = this.records.PlacedID;

                if (placeID == Guid.Empty)
                    return null;

                if (this.linkedLayer == null)
                {
                    this.linkedLayer = this.document.LinkedLayers.Where(i => i.ID == placeID && i.HasDocument).FirstOrDefault();
                }
                return this.linkedLayer;
            }
        }

        public bool HasImage
        {
            get
            {
                if (this.records.SectionType != SectionType.Normal)
                    return false;
                if (this.Width == 0 || this.Height == 0)
                    return false;
                return true;
            }
        }

        public bool HasMask
        {
            get { return this.records.Mask != null; }
        }

        public void ReadChannels(PsdReader reader)
        {
            this.channels = new ChannelsBlock(reader, this.records.ChannelSize, this);
        }

        public void ComputeBounds()
        {
            SectionType sectionType = this.records.SectionType;
            if (sectionType != SectionType.Opend && sectionType != SectionType.Closed)
                return;

            int left = int.MaxValue;
            int top = int.MaxValue;
            int right = int.MinValue;
            int bottom = int.MinValue;

            bool isSet = false;

            foreach (var item in this.Descendants())
            {
                if (item == this || item.HasImage == false)
                    continue;

                var block = item.Resources.FirstOrDefault(x => x is PlLdResource) as PlLdResource;

                if (block != null)
                {
                    double[] transforms = block.Transformation;
                    double[] xx = new double[] { transforms[0], transforms[2], transforms[4], transforms[6], };
                    double[] yy = new double[] { transforms[1], transforms[3], transforms[5], transforms[7], };

                    int l = (int)Math.Ceiling(xx.Min());
                    int r = (int)Math.Ceiling(xx.Max());
                    int t = (int)Math.Ceiling(yy.Min());
                    int b = (int)Math.Ceiling(yy.Max());
                    left = Math.Min(l, left);
                    top = Math.Min(t, top);
                    right = Math.Max(r, right);
                    bottom = Math.Max(b, bottom);
                }
                else
                {
                    left = Math.Min(item.Left, left);
                    top = Math.Min(item.Top, top);
                    right = Math.Max(item.Right, right);
                    bottom = Math.Max(item.Bottom, bottom);
                }
                isSet = true;
            }

            if (isSet == false)
                return;

            this.left = left;
            this.top = top;
            this.right = right;
            this.bottom = bottom;
        }

        #region IPsdLayer

        IPsdLayer IPsdLayer.Parent
        {
            get
            {
                if (this.parent == null)
                    return this.document;
                return this.parent;
            }
        }

        IChannel[] IImageSource.Channels
        {
            get { return this.channels.Channels; }
        }

        IPsdLayer[] IPsdLayer.Childreen
        {
            get { return this.Childreen; }
        }

        #endregion
    }
}

