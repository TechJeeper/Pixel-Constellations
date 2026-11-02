module.exports = [
  {
    type: 'heading',
    defaultValue: 'Pixel Constellations'
  },
  {
    type: 'text',
    defaultValue: 'A night sky where stars slowly connect into constellations that spell the time.'
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Time'
      },
      {
        type: 'select',
        messageKey: 'TimeFormat',
        defaultValue: '0',
        label: 'Hour format',
        options: [
          { label: 'System default', value: '0' },
          { label: '12-hour', value: '1' },
          { label: '24-hour', value: '2' }
        ]
      },
      {
        type: 'toggle',
        messageKey: 'LeadingZero',
        label: 'Leading zero on hours',
        defaultValue: true
      },
      {
        type: 'toggle',
        messageKey: 'ShowDate',
        label: 'Show date',
        defaultValue: false
      },
      {
        type: 'toggle',
        messageKey: 'ShowSeconds',
        label: 'Blink colon with seconds',
        defaultValue: true
      }
    ]
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Sky'
      },
      {
        type: 'select',
        messageKey: 'ColorTheme',
        defaultValue: '0',
        label: 'Color theme',
        options: [
          { label: 'Classic night', value: '0' },
          { label: 'Gold', value: '1' },
          { label: 'Cyan', value: '2' },
          { label: 'Magenta', value: '3' },
          { label: 'Aurora', value: '4' }
        ]
      },
      {
        type: 'toggle',
        messageKey: 'ShowMoon',
        label: 'Show moon',
        defaultValue: true
      },
      {
        type: 'toggle',
        messageKey: 'ShowNebula',
        label: 'Show nebula',
        defaultValue: true
      },
      {
        type: 'select',
        messageKey: 'AmbientStars',
        defaultValue: '1',
        label: 'Background stars',
        options: [
          { label: 'Sparse', value: '0' },
          { label: 'Medium', value: '1' },
          { label: 'Dense', value: '2' }
        ]
      },
      {
        type: 'select',
        messageKey: 'Twinkle',
        defaultValue: '1',
        label: 'Twinkle',
        options: [
          { label: 'Soft', value: '0' },
          { label: 'Medium', value: '1' },
          { label: 'Strong', value: '2' }
        ]
      }
    ]
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Constellations'
      },
      {
        type: 'select',
        messageKey: 'AnimSpeed',
        defaultValue: '1',
        label: 'Line draw speed',
        options: [
          { label: 'Slow', value: '0' },
          { label: 'Normal', value: '1' },
          { label: 'Fast', value: '2' },
          { label: 'Instant', value: '3' }
        ]
      }
    ]
  },

  {
    type: 'submit',
    defaultValue: 'Save'
  }
];
